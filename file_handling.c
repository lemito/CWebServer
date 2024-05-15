//
// Created by lemito on 5/15/24.
//

#include "file_handling.h"

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