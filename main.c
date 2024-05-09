#include <arpa/inet.h>
#include <errno.h>
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

char *response_creator(const int status, const char *content_type, const char *message)
{
    char *response = malloc(4096);
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
    size_t response_len = message_len + 6;
    char *response = malloc(response_len);
    if (response == NULL)
    {
        fprintf(stderr, "Не удалось создать response");
        return NULL;
    }

    snprintf(response, response_len, "<p>%s</p>", message);
    return response;
}



int main()
{
    char buffer[BUFFER_SIZE];
    char *test_resp = response_creator(200, "text/html", text_creator("Hello, world!"));
    printf("%s", test_resp);

    // создаем сокет ~= "шина" клиент-сервер
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Ошибка");
        exit(EXIT_FAILURE);
    }
    puts("Сокет успешно создан\n");

    // создаем адрес хоста
    struct sockaddr_in host_addr;
    socklen_t host_addr_len = sizeof(host_addr);

    // инициализируем адрес
    // используем ipv4
    host_addr.sin_family = AF_INET;
    // сидим на определенном порту; содержится в виде 4 байт сети
    host_addr.sin_port = htons(PORT);
    // сидим на 0.0.0.0
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&host_addr, host_addr_len))
    {
        perror("Не удалось связать адрес и сокет");
        exit(EXIT_FAILURE);
    }
    puts("Связано успешно!");

    // прослушиваем входящий подключения; не мелочимся и берем максимум
    if (listen(sockfd, SOMAXCONN) != 0)
    {
        perror("Прослушивание неудачно");
        exit(EXIT_FAILURE);
    }

    puts("Сервер готов к пролушиванию соединений");

    printf("Сервер готов по адресу 0.0.0.0:8080\n");

    while (1)
    {
        // принимаем входящие подключения
        int new_sockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addr_len);

        if (new_sockfd < 0)
        {
            perror("Ошибка при подключении");
        }

        puts("Кто-то подключился | Успешно!");

        // читаем с потока в буфер
        ssize_t valRead = read(new_sockfd, buffer, BUFFER_SIZE);
        if (valRead < 0)
        {
            perror("Чтение не успешно");
            close(new_sockfd);
            continue;
        }

        // пишем в поток
        ssize_t valWrite = write(new_sockfd, test_resp, strlen(test_resp));
        if (valWrite < 0)
        {
            perror("Запись не успешна");
            close(new_sockfd);
            continue;
        }

        // закрываем подключение
        close(new_sockfd);
    }

    FREE_AND_NULL(test_resp);

    return 0;
}