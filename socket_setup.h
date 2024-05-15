//
// Created by lemito on 5/15/24.
//

#ifndef CWEBSERVER_SOCKET_SETUP_H
#define CWEBSERVER_SOCKET_SETUP_H

#include "config.h"
#include <netinet/in.h>
#include "logger.h"

int setup_socket(void);
void setup_server(int sockfd);
void shutdown_server(int sockfd);

#endif //CWEBSERVER_SOCKET_SETUP_H
