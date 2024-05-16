//
// Created by lemito on 5/15/24.
//

#include "socket_setup.h"

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
    unsigned int host_addr_len = sizeof(host_addr);
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

void shutdown_server(int sockfd) {
    close(sockfd);
    log_write("Сервер успешно остановлен.");
}
