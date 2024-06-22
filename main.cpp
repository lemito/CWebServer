#include <iostream>
#include <fstream>
#include <csignal>
#include <thread>
#include <memory>
#include <algorithm>

#include "handlers.h"
#include "logger.h"
#include "socket_setup.h"

// для POSIX систем
#ifdef _POSIX_C_SOURCE
#define USE_SIGACTION

#endif

FILE *log_file = nullptr;
bool server_running = true;

void handle_signal(int sig)
{
    if (sig == SIGINT)
    {
        server_running = false;
        log_write("Сигнал остановки получен. Сервер останавливается...");
    }
}

int main()
{
    std::signal(SIGINT, handle_signal);

    int sockfd = setup_socket();
    setup_server(sockfd);

    log_file = log_file_init();
    if (log_file == nullptr)
    {
        perror("Ошибка при инициализации файла лога");
        return 1;
    }
    log_write("Сервер успешно запущен!");

    while (server_running)
    {
        struct sockaddr_in host_addr;
        unsigned int host_addr_len = sizeof(host_addr);
        int new_sockfd = accept(sockfd, (struct sockaddr *)&host_addr, &host_addr_len);
        if (new_sockfd < 0)
        {
            perror("Ошибка при подключении");
            log_write("Ошибка при подключении");
            continue;
        }
        log_write("Кто-то подключился | Успешно!");

        std::unique_ptr<std::thread> client_thread(new std::thread([new_sockfd]() {
            handle_client_thread(reinterpret_cast<void*>(new_sockfd));
        }));
        client_thread->detach();
    }
    shutdown_server(sockfd);
    log_write("Сервер выключен!");
    fclose(log_file);
    return 0;
}
