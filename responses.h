#ifndef CWEBSERVER_RESPONSES_H
#define CWEBSERVER_RESPONSES_H

#include "logger.h"
#include "config.h"

#ifdef __cplusplus
extern "C"
{
#endif

char *response_creator(int status, const char *content_type, const char *message);
char *response_creator_static(int status, const char *content_type, size_t content_length);
void write_response(int sockfd, char* response, size_t len);

#ifdef __cplusplus
}
#endif

#endif //CWEBSERVER_RESPONSES_H
