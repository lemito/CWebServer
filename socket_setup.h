//
// Created by lemito on 5/15/24.
//

#ifndef CWEBSERVER_SOCKET_SETUP_H
#define CWEBSERVER_SOCKET_SETUP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "config.h"
#ifdef _POSIX_C_SOURCE
#include <netinet/in.h>
#include <unistd.h>
#elif defined(WIN32)
#include <winsock2.h>
#endif
#include "logger.h"

int setup_socket(void);
void setup_server(int sockfd);
void shutdown_server(int sockfd);

#ifdef __cplusplus
}
#endif



#endif //CWEBSERVER_SOCKET_SETUP_H
