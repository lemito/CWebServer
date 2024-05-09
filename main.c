#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main()
{
    // создаем сокет ~= "шина" клиент-сервер
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Ошибка");
        return 1;
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
        return 1;
    }
    puts("Связано успешно!");

    // прослушиваем входящий подключения; не мелочимся и берем максимум
    if (listen(sockfd, SOMAXCONN) != 0)
    {
        perror("Прослушивание неудачно");
    }

    puts("Сервер готов к пролушиванию соединений");

    for (;;)
    {
        // принимаем входящие подключения
        int new_sockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addr_len);

        if (new_sockfd < 0){
            perror("Ошибка при подключении");
        }

        puts("Соединение принято");

        // закрываем подключение
        close(new_sockfd);
    }

    return 0;
}