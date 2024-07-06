#ifndef CWEBSERVER_HANDLERS_H
#define CWEBSERVER_HANDLERS_H

#include "responses.h"
#include "logger.h"
#include "config.h"
#include "html_helpers.h"
#include "utils.h"

#ifdef __cplusplus
extern "C"
{
#endif

void handle_home(int new_sockfd, char *buffer);
void handle_about(int sockfd);
void handle_404(int sockfd);
HTTP_METHODS method(char *buffer);
void* handle_client_thread(void* arg);
void router(char *route, int sockfd, char *buffer);

#ifdef __cplusplus
}
#endif


#endif //CWEBSERVER_HANDLERS_H
