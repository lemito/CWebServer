#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include "responses.h"
#include "handlers.h"
#include "logger.h"


FILE *log_file = NULL;
volatile sig_atomic_t server_running = 1;

void handle_signal(int sig) {
    if (sig == SIGINT) {
        server_running = 0;
    }
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
    memset(&host_addr, 0, sizeof(host_addr));
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
    write_response(sockfd, response, strlen(response));
    FREE_AND_NULL(response);

    lseek(filefd, 0, SEEK_SET);

    while ((bytes_read = read(filefd, buffer, sizeof(buffer))) > 0)
    {
        write_response(sockfd, buffer, bytes_read);
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
            write_response(sockfd, response, strlen(response));
            FREE_AND_NULL(response);
        }
    }
    else if (strcmp(route, "/favicon.ico") == 0)
    {
        char *response = response_creator(HTTP_NOT_FOUND, "text/plain", "Favicon not available");
        write_response(sockfd, response, strlen(response));
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
    if (write(sockfd, data, size) < 0)
        fprintf(stderr, "Ошибка записи html вывода");
}

void html(int sockfd, const char *txt)
{
    size_t txt_len = strlen(txt);
    html_raw(sockfd, txt, txt_len);
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
    signal(SIGINT, handle_signal);

    int sockfd = setup_socket();
    setup_server(sockfd);

    log_file = log_file_init();
    log_write("Сервер успешно запущен!");

    while (server_running)
    {
        struct sockaddr_in host_addr;
        socklen_t host_addr_len = sizeof(host_addr);
        int new_sockfd = accept(sockfd, (struct sockaddr *)&host_addr, &host_addr_len);
        if (new_sockfd < 0)
        {
            perror("Ошибка при подключении");
            log_write( "Ошибка при подключении");
            continue;
        }
        puts("Кто-то подключился | Успешно!");
        log_write( "Кто-то подключился | Успешно!");
        handle_client(new_sockfd);
    }

    log_write( "Сервер выключен!");
    fclose(log_file);
    close(sockfd);
    return 0;
}
