#include <algorithm>
#include <csignal>
#include <fstream>
#include <iostream>
#include <thread>

#include "absl/log/log.h"
#include "absl/strings/str_format.h"
#include "handlers.h"
#include "logger.h"
#include "socket_setup.h"

// для POSIX систем
#ifdef _POSIX_C_SOURCE
#define USE_SIGACTION
#endif

FILE *log_file = nullptr;
bool server_running = true;

void handle_signal(int sig) {
  if (sig == SIGINT) {
    server_running = false;
    LOG(INFO) << "Сигнал остановки получен. Сервер останавливается...";
  }
}

int main() {
  constexpr absl::string_view kFormatString = "Welcome to %s, Number %d!";
  std::string s = absl::StrFormat(kFormatString, "The Village", 6);

  absl::PrintF("Trying to request TITLE: %s \n", s);

  std::signal(SIGINT, handle_signal);

  int sockfd = setup_socket();
  setup_server(sockfd);

  log_file = log_file_init();
  if (log_file == nullptr) {
    perror("Ошибка при инициализации файла лога");
    return 1;
  }
  LOG(INFO) << "Сервер успешно запущен!";

  while (server_running) {
    struct sockaddr_in host_addr;
    unsigned int host_addr_len = sizeof(host_addr);
    int new_sockfd =
        accept(sockfd, (struct sockaddr *)&host_addr, &host_addr_len);
    if (new_sockfd < 0) {
      perror("Ошибка при подключении");
      LOG(INFO) << "Ошибка при подключении";

      continue;
    }
    LOG(INFO) << "Кто-то подключился | Успешно!";

    std::shared_ptr<int> new_sockfd_shared;
    new_sockfd_shared = std::make_shared<int>(new_sockfd);
    std::unique_ptr<std::thread> client_thread(
        new std::thread(handle_client_thread, new_sockfd_shared.get()));
    client_thread->detach();
  }
  shutdown_server(sockfd);
  LOG(INFO) << "Сервер выключен!";

  fclose(log_file);
  return 0;
}
