#ifndef CWEBSERVER_HANDLERS_H
#define CWEBSERVER_HANDLERS_H

#include "main.h"
#include "responses.h"
#include "logger.h"


void handle_home(int new_sockfd, char *buffer);
void handle_about(int sockfd);
void handle_404(int sockfd);
HTTP_METHODS method(char *buffer);
void handle_client(int new_sockfd);

#endif //CWEBSERVER_HANDLERS_H
