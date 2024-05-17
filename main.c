#include "handlers.h"
#include "logger.h"
#include "socket_setup.h"


FILE *log_file = NULL;
volatile sig_atomic_t server_running = 1;

void handle_signal(int sig) {
    if (sig == SIGINT) {
        server_running = 0;
        log_write("Сигнал остановки получен. Сервер останавливается...");
    }
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
        unsigned int host_addr_len = sizeof(host_addr);
        int new_sockfd = accept(sockfd, (struct sockaddr *)&host_addr, &host_addr_len);
        if (new_sockfd < 0)
        {
            perror("Ошибка при подключении");
            log_write( "Ошибка при подключении");
            continue;
        }
        log_write( "Кто-то подключился | Успешно!");
        handle_client(new_sockfd);
    }
    shutdown_server(sockfd);
    log_write( "Сервер выключен!");
    fclose(log_file);
//    close(sockfd);
    return 0;
}
