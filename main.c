#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define FREE_AND_NULL(p) \
    do                   \
    {                    \
        free(p);         \
        (p) = NULL;      \
    } while (0)

// HTTP коды
#define HTTP_OK 200
#define HTTP_NOT_FOUND 404
#define HTTP_BAD_REQUEST 400
#define HTTP_NOT_ALLOWED 405
// HTTP статусы текстом
#define STATUS_TEXT_OK "OK"
#define STATUS_TEXT_NOT_FOUND "Not Found"
#define STATUS_TEXT_BAD_REQUEST "Bad Request"
#define STATUS_TEXT_NOT_ALLOWED "Method Not Allowed"

#define JPEG "image/jpeg"
#define JS "text/javascript"
#define CSS "text/css"

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

volatile sig_atomic_t server_running = 1;

void handle_signal(int sig) {
    if (sig == SIGINT) {
        server_running = 0;
    }
}

char *response_creator(const int status, const char *content_type, const char *message)
{
    char *response = malloc(BUFFER_SIZE);
    if (response == NULL)
    {
        fprintf(stderr, "Не удалось создать response");
        return NULL;
    }
    const char* status_text;
    switch(status) {
        case HTTP_OK:
            status_text = STATUS_TEXT_OK;
            break;
        case HTTP_NOT_FOUND:
            status_text = STATUS_TEXT_NOT_FOUND;
            break;
        case HTTP_BAD_REQUEST:
            status_text = STATUS_TEXT_BAD_REQUEST;
            break;
        case HTTP_NOT_ALLOWED:
            status_text = STATUS_TEXT_NOT_ALLOWED;
            break;
        default:
            status_text = "Unknown Status";
    }
    snprintf(response, 4096, "HTTP/1.0 %d %s\r\n"
                             "Server: CWebServer\r\n"
                             "Content-length: %lu\r\n"
                             "Content-type: %s\r\n\r\n"
                             "<html>%s</html>\r\n",
             status, status_text, strlen(message) + 13, content_type, message);
    return response;
}

char *response_creator_static(const int status, const char *content_type, size_t content_length)
{
    char *response = malloc(BUFFER_SIZE);
    if (response == NULL)
    {
        fprintf(stderr, "Не удалось создать response");
        return NULL;
    }
    snprintf(response, 4096, "HTTP/1.0 %d OK\r\n"
                             "Server: CWebServer-STATIC\r\n"
                             "Content-length: %lu\r\n"
                             "Content-type: %s\r\n\r\n",
             status, content_length, content_type);
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
//    LOG("Сокет успешно создан");
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
    puts("Связано успешно");

    if (listen(sockfd, SOMAXCONN) != 0)
    {
        perror("Прослушивание неудачно");
        exit(EXIT_FAILURE);
    }
    puts("Сервер запущен!");
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

void send_static_file(int sockfd, const char *filename)
{
    int filefd = open(filename, O_RDONLY);
    if (filefd == -1)
    {
        perror("Ошибка открытия файла");
        return;
    }

    char *content_type = "text/plain";
    if (strstr(filename, ".css"))
        content_type = CSS;
    else if (strstr(filename, ".js"))
        content_type = JS;
    else if (strstr(filename, ".jpeg"))
        content_type = JPEG;

    char buffer[1024];
    ssize_t bytes_read;
    size_t total_bytes_read = 0;

    while ((bytes_read = read(filefd, buffer, sizeof(buffer))) > 0)
    {
        total_bytes_read += bytes_read;
    }

    char *response = response_creator_static(HTTP_OK, content_type, total_bytes_read);
    write(sockfd, response, strlen(response));
    FREE_AND_NULL(response);

    lseek(filefd, 0, SEEK_SET);

    while ((bytes_read = read(filefd, buffer, sizeof(buffer))) > 0)
    {
        write(sockfd, buffer, bytes_read);
    }

    close(filefd);
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
            char *response = response_creator(HTTP_NOT_ALLOWED, "text/plain", "Метод запрещен для данного пути");
            write(sockfd, response, strlen(response));
            FREE_AND_NULL(response);
        }
    }
    else if (strcmp(route, "/favicon.ico") == 0)
    {
        char *response = response_creator(HTTP_NOT_FOUND, "text/plain", "Favicon not available");
        write(sockfd, response, strlen(response));
        FREE_AND_NULL(response);
    }
    else if (strncmp(route, "/static/", 8) == 0)
    {
        if (route != NULL) {
            char *filename = malloc(strlen(route) + 1);
            if (filename == NULL) {
                return;
            }
            strcpy(filename, ".");
            strcat(filename, route);
            send_static_file(sockfd, filename);
            free(filename);
//            FREE_AND_NULL(filename);
        }
    }
    else
    {
        handle_404(sockfd);
    }

    //    FREE_AND_NULL(buffer);
}

void html_raw(int sockfd, const char* data, size_t size){
    if (write(sockfd, data, size) != size)
        fprintf(stderr, "Ошибка записи html вывода");
}

void html(int sockfd, const char *txt)
{
    html_raw(sockfd, txt, strlen(txt));
}

void html_attr(int sockfd, const char *txt)
{
    const char *t = txt;
    while (t && *t) {
        int c = (int)*t;
        if (c == '<' || c == '>' || c == '\'' || c == '\"' || c == '&') {
            html_raw(sockfd, txt, t - txt);
            if (c == '>')
                html(sockfd, "&gt;");
            else if (c == '<')
                html(sockfd, "&lt;");
            else if (c == '\'')
                html(sockfd, "&#x27;");
            else if (c == '"')
                html(sockfd, "&quot;");
            else html(sockfd, "&amp;");
            txt = t + 1;
        }
        t++;
    }
    if (t != txt)
        html(sockfd, txt);
}

void html_link_open(int sockfd, const char *url, const char *title, const char *class)
{
    html(sockfd, "<a href='");
    html_attr(sockfd, url);
    if (title) {
        html(sockfd, "' title='");
        html_attr(sockfd, title);
    }
    if (class) {
        html(sockfd, "' class='");
        html_attr(sockfd, class);
    }
    html(sockfd, "'>");
}

void html_link_close(int sockfd)
{
    html(sockfd, "</a>");
}

void handle_home(int new_sockfd, char *buffer)
{
    enum HTTP_METHODS req_method = method(buffer);
    if (req_method == GET)
    {
        // Отправка формы на главную страницу
        char *form = form_creator("/", "", "name");
        char *test_resp = response_creator(HTTP_OK, "text/html", form);
        ssize_t valWrite = write(new_sockfd, test_resp, strlen(test_resp));
        if (valWrite < 0)
        {
            perror("Запись не успешна");
        }
        FREE_AND_NULL(form);
        FREE_AND_NULL(test_resp);
    }
    else if (req_method == POST)
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
                    name_end = strchr(name_start, '\0');

                if (name_end - name_start > 99)
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
                char *response = response_creator(HTTP_OK, "text/html", greeting);
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

    char *response = response_creator(HTTP_NOT_FOUND, "text/html", message);
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

    char *response = response_creator(HTTP_NOT_FOUND, "text/html", html);
    printf("404 = %s\n", response);

    write(sockfd, response, strlen(response));

    FREE_AND_NULL(response);
    FREE_AND_NULL(html);
    fclose(aboutfd);
}

enum HTTP_METHODS method(char *buffer)
{
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
    FREE_AND_NULL(url);
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

FILE *log_file_init()
{
    time_t rawtime;
    struct tm *timeinfo;
    char filename[45];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(filename, "LOG-%d-%02d-%02d_%02d-%02d-%02d.txt",
            1900 + timeinfo->tm_year, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Ошибка открытия файла.\n");
        return (FILE *)NULL;
    }

    printf("Лог-файл с именем %s успешно создан.\n", filename);

    return file;
}

int main()
{
    signal(SIGINT, handle_signal);

    int sockfd = setup_socket();
    setup_server(sockfd);

    FILE* logfilefd = log_file_init();
    if (logfilefd != NULL) {
        fprintf(logfilefd, "Сервер успешно запущен!\n");
        fflush(logfilefd);
    }

    while (server_running)
    {
        struct sockaddr_in host_addr;
        socklen_t host_addr_len = sizeof(host_addr);
        int new_sockfd = accept(sockfd, (struct sockaddr *)&host_addr, &host_addr_len);
        if (new_sockfd < 0)
        {
            perror("Ошибка при подключении");
            if (logfilefd != NULL) {
                fprintf(logfilefd, "Ошибка при подключении\n");
                fflush(logfilefd);
            }
            continue;
        }
        puts("Кто-то подключился | Успешно!");
        if (logfilefd != NULL) {
            fprintf(logfilefd, "Кто-то подключился | Успешно!\n");
            fflush(logfilefd);
        }
        handle_client(new_sockfd);
    }

    if (logfilefd != NULL) {
        fprintf(logfilefd, "Сервер выключен!\n");
        fflush(logfilefd);
        fclose(logfilefd);
    }

    close(sockfd);
    return 0;
}
