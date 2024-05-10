#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define FREE_AND_NULL(p) \
    do                   \
    {                    \
        free(p);         \
        (p) = NULL;      \
    } while (0)

// HTTP коды
#define OK 200
#define NOTFOUND 404
#define BAD_REQUEST 400
#define NOT_ALLOWED 405

enum HTTP_METHODS
{
    GET,
    POST,
    DELETE,
    PUT
};

char *parse_url(char *buffer);
void handle_home(int new_sockfd, char *buffer);
void handle_about(int sockfd);
void handle_404(int sockfd);
enum HTTP_METHODS method(char *buffer);

char *response_creator(const int status, const char *content_type, const char *message)
{
    char *response = malloc(BUFFER_SIZE);
    if (response == NULL)
    {
        fprintf(stderr, "Не удалось создать response");
        return NULL;
    }
    snprintf(response, 4096, "HTTP/1.0 %d OK\r\n"
                             "Server: CWebServer\r\n"
                             "Content-type: %s\r\n\r\n"
                             "<html>%s</html>\r\n",
             status, content_type, message);
    return response;
}

char *text_creator(const char *message)
{
    size_t message_len = strlen(message);
    size_t response_len = message_len + 8;
    char *response = malloc(response_len);
    if (response == NULL)
    {
        fprintf(stderr, "Не удалось создать response");
        return NULL;
    }

    snprintf(response, response_len, "<p>%s</p>", message);
    return response;
}

char *form_creator(const char *action, char *const value, char *const id)
{
    char *response = malloc(1024);

    snprintf(response, 1024, "<form action='%s' method='post'><input value='%s' id='%s' type='text' name='name' /><button type='submit'>Submit</button></form>", action, value, id);
    return response;
}

int setup_socket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Ошибка");
        exit(EXIT_FAILURE);
    }
    puts("Сокет успешно создан");
    return sockfd;
}

void setup_server(int sockfd)
{
    struct sockaddr_in host_addr;
    socklen_t host_addr_len = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&host_addr, host_addr_len))
    {
        perror("Не удалось связать адрес и сокет");
        exit(EXIT_FAILURE);
    }
    puts("Связано успешно!");

    if (listen(sockfd, SOMAXCONN) != 0)
    {
        perror("Прослушивание неудачно");
        exit(EXIT_FAILURE);
    }
    puts("Сервер готов к пролушиванию соединений");
    printf("Сервер готов по адресу 0.0.0.0:8080\n");
}

char *read_all_from_html(FILE *fd)
{
    size_t bufSize, pos = 0;
    char *buf, *newBuf;

    fseek(fd, 0, SEEK_END);
    bufSize = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    buf = malloc(bufSize);
    if (buf == NULL)
    {
        return NULL;
    }

    while (pos < bufSize)
    {
        size_t bytesRead = fread(buf + pos, sizeof(char), bufSize - pos, fd);
        if (bytesRead == 0)
        {
            if (feof(fd))
            {
                break;
            }
            else
            {
                free(buf);
                return NULL;
            }
        }
        pos += bytesRead;
    }

    newBuf = realloc(buf, pos);
    if (newBuf != NULL)
    {
        buf = newBuf;
    }

    return buf;
}

void router(char *route, int sockfd, char *buffer)
{
    if (strcmp(route, "/") == 0)
    {
        handle_home(sockfd, buffer);
    }
    else if (strcmp(route, "/about") == 0)
    {
        if (method(buffer) == GET)
            handle_about(sockfd);
        else
        {
            char *response = response_creator(NOT_ALLOWED, "text/plain", "Метод запрещен для данного пути");
            write(sockfd, response, strlen(response));
            FREE_AND_NULL(response);
        }
    }
    else if (strcmp(route, "/favicon.ico") == 0)
    {
        char *response = response_creator(NOTFOUND, "text/plain", "Favicon not available");
        write(sockfd, response, strlen(response));
        FREE_AND_NULL(response);
    }
    else
    {
        handle_404(sockfd);
    }

    //    FREE_AND_NULL(buffer);
}

void handle_home(int new_sockfd, char *buffer)
{

    if (method(buffer) == GET)
    {
        // Отправка формы на главную страницу
        char *test_resp = response_creator(OK, "text/html", form_creator("/", "", "name"));
        ssize_t valWrite = write(new_sockfd, test_resp, strlen(test_resp));
        if (valWrite < 0)
        {
            perror("Запись не успешна");
        }
        FREE_AND_NULL(test_resp);
    }
    else if (method(buffer) == POST)
    {
        // Обработка данных формы
        char *body = strstr(buffer, "\r\n\r\n");
        if (body)
        {
            body += 4;
            char *name_start = strstr(body, "name=");
            if (name_start)
            {
                name_start += 5;
                char *name_end = strchr(name_start, '&');
                if (!name_end)
                    name_end = strchr(name_start, '\0'); // Ensure we don't go past the end of the string

                if (name_end - name_start > 99) // Check to ensure we don't overflow the name buffer
                {
                    fprintf(stderr, "Name value too long\n");
                    return;
                }

                char name[100];
                strncpy(name, name_start, name_end - name_start);
                name[name_end - name_start] = '\0';

                // Создание сообщения приветствия с именем пользователя
                char *greeting_msg = strdup("Hello ");
                if (!greeting_msg)
                {
                    fprintf(stderr, "Не удалось выделить память\n");
                    return;
                }

                strncat(greeting_msg, name, sizeof(name) - strlen(greeting_msg) - 1);
                char *greeting = text_creator(greeting_msg);
                char *response = response_creator(OK, "text/html", greeting);
                printf("response form = %s\n", response);

                ssize_t valWrite = write(new_sockfd, response, strlen(response));
                if (valWrite < 0)
                {
                    perror("Запись не успешна");
                }
                FREE_AND_NULL(greeting_msg);
                FREE_AND_NULL(response);
                FREE_AND_NULL(greeting);
            }
        }
    }
}

void handle_404(int sockfd)
{
    char *message = malloc(strlen("<h1>404</h1>") + strlen(text_creator("Page not found")) + 1);
    if (message == NULL)
    {
        perror("Не удалось выделить память под сообщение 404");
        return;
    }

    strcpy(message, "<h1>404</h1>");
    strcat(message, text_creator("Page not found"));

    char *response = response_creator(NOTFOUND, "text/html", message);
    printf("404 = %s\n", response);

    write(sockfd, response, strlen(response));

    FREE_AND_NULL(response);
    FREE_AND_NULL(message);
}

void handle_about(int sockfd)
{
    FILE *aboutfd = fopen("about.html", "r");
    if (aboutfd == NULL)
    {
        fprintf(stderr, "Не удалось найти файл about.html");
    }
    char *html = read_all_from_html(aboutfd);
    if (html == NULL)
    {
        perror("Не удалось выделить память под сообщение 404");
        return;
    }

    char *response = response_creator(OK, "text/html", html);
    printf("404 = %s\n", response);

    write(sockfd, response, strlen(response));

    FREE_AND_NULL(response);
    FREE_AND_NULL(html);
    fclose(aboutfd);
}

enum HTTP_METHODS method(char *buffer)
{
    printf("buffer:\n%s", buffer);
    enum HTTP_METHODS result;
    if (strncmp(buffer, "POST", 4) == 0)
    {
        result = POST;
    }
    else if (strncmp(buffer, "GET", 3) == 0)
    {
        result = GET;
    }
    else if (strncmp(buffer, "PUT", 3) == 0)
    {
        result = PUT;
    }
    else if (strncmp(buffer, "DELETE", 6) == 0)
    {
        result = DELETE;
    }
    else
    {
        fprintf(stderr, "Не удалось распознать метод");
        exit(EXIT_FAILURE);
    }

    return result;
}

void handle_client(int new_sockfd)
{
    char *buffer = malloc(BUFFER_SIZE);

    ssize_t valRead = read(new_sockfd, buffer, BUFFER_SIZE);
    if (valRead < 0)
    {
        perror("Чтение не успешно");
        close(new_sockfd);
        return;
    }

    char *url;
    url = parse_url(buffer);

    printf("url = %s ; len = %lu\n", url, strlen(url));

    router(url, new_sockfd, buffer);

    close(new_sockfd);
    FREE_AND_NULL(buffer);
}

char *parse_url(char *buffer)
{
    // GET /122 HTTP/1.1
    //    |    |
    // нам нужно забрать всё, что между палочками -- это url
    // первый пробел
    char *method_end = strchr(buffer, ' ');
    if (!method_end)
    {
        fprintf(stderr, "Неверный формат запроса\n");
        return NULL;
    }

    // "/" ~= начало url
    char *url_start = method_end + 1;

    // второй пробел
    char *url_end = strchr(url_start, ' ');
    if (!url_end)
    {
        fprintf(stderr, "Неверный формат запроса\n");
        return NULL;
    }

    // длина url
    size_t url_length = url_end - url_start;

    // создаем строку для url
    char *url = malloc(url_length + 1);
    if (!url)
    {
        fprintf(stderr, "Не удалось выделить память\n");
        return NULL;
    }
    strncpy(url, url_start, url_length);
    url[url_length] = '\0';

    return url;
}

int main()
{
    int sockfd = setup_socket();
    setup_server(sockfd);

    while (1)
    {
        struct sockaddr_in host_addr;
        socklen_t host_addr_len = sizeof(host_addr);
        int new_sockfd = accept(sockfd, (struct sockaddr *)&host_addr, &host_addr_len);
        if (new_sockfd < 0)
        {
            perror("Ошибка при подключении");
            exit(EXIT_FAILURE);
        }
        puts("Кто-то подключился | Успешно!");
        handle_client(new_sockfd);
    }
    // FREE_AND_NULL(test_resp);
}
