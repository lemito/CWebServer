#include "responses.h"


char *response_creator(const int status, const char *content_type, const char *message)
{
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
    char *response = malloc(BUFFER_SIZE);
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

void write_response(int sockfd, char* response, size_t len){
    ssize_t bytes = write(sockfd, response, len);
    if (bytes < 0){
        perror("Запись ответа не успешна!");
        log_write("Запись ответа не успешна!");
    }
}