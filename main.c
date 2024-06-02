#include "handlers.h"
#include "logger.h"
#include "socket_setup.h"

// для POSIX систем
#ifdef _POSIX_C_SOURCE
#define USE_SIGACTION
#include <signal.h>
#endif

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

    #if defined(USE_SIGACTION)
        struct sigaction sa;
        sa.sa_handler = handle_signal;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);
    #else
        signal(SIGINT, handle_signal);
    #endif

    int sockfd = setup_socket();
    setup_server(sockfd);

    log_file = log_file_init();
    if (log_file == NULL) {
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
            log_write( "Ошибка при подключении");
            continue;
        }
        log_write( "Кто-то подключился | Успешно!");
        handle_client(new_sockfd);
        close(new_sockfd);
    }
    shutdown_server(sockfd);
    log_write( "Сервер выключен!");
    fclose(log_file);
    return 0;
}
