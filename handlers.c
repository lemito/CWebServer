#include "handlers.h"



void handle_home(int new_sockfd, char *buffer)
{
    HTTP_METHODS req_method = method(buffer);
    if (req_method == GET)
    {
        char *title = strdup("<h1>Home</h1>");
        char* form = malloc(100);
        char *resp_msg;

        strcpy(form, form_creator("/", "", "name"));

        resp_msg = malloc(strlen(title) + strlen(form) + 1);
        strcpy(resp_msg, title);
        strcat(resp_msg, form);

        char *test_resp = response_creator(HTTP_OK, "text/html", resp_msg);

        printf("resp = %s", test_resp);

        write_response(new_sockfd, test_resp, strlen(test_resp));

        FREE_AND_NULL(title);
        FREE_AND_NULL(form);
        FREE_AND_NULL(resp_msg);
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

                char *greeting_msg = strdup("Hello ");
                if (!greeting_msg)
                {
                    fprintf(stderr, "Не удалось выделить память\n");
                    return;
                }

                strncat(greeting_msg, name, sizeof(name) - strlen(greeting_msg) - 1);
                char *greeting = text_creator(greeting_msg);
                char *response = response_creator(HTTP_OK, "text/html", greeting);

                write_response(new_sockfd, response, strlen(response));

                FREE_AND_NULL(greeting_msg);
                FREE_AND_NULL(response);
                FREE_AND_NULL(greeting);
            }
        }
        FREE_AND_NULL(body);
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

    write_response(sockfd, response, strlen(response));

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

    char *response = response_creator(HTTP_OK, "text/html", html);

    write_response(sockfd, response, strlen(response));

    FREE_AND_NULL(response);
    FREE_AND_NULL(html);
    fclose(aboutfd);
}

HTTP_METHODS method(char *buffer)
{
    HTTP_METHODS result;
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

    char *log_msg = malloc(256 * sizeof (char));
    snprintf(log_msg, 256, "Кто-то подключился на url %s, номер сокета = %d", url, new_sockfd);
    log_write(log_msg);
    FREE_AND_NULL(log_msg);

    router(url, new_sockfd, buffer);

    close(new_sockfd);
    FREE_AND_NULL(url);
    FREE_AND_NULL(buffer);
}