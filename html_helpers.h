//
// Created by lemito on 5/15/24.
//

#ifndef CWEBSERVER_HTML_HELPERS_H
#define CWEBSERVER_HTML_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
// #include "handlers.h"
#include "file_handling.h"

void html_raw(int sockfd, const char *data, size_t size);
void html(int sockfd, const char *txt);
void html_attr(int sockfd, const char *txt);

//__attribute__((unused)) void html_link_open(int sockfd, const char *url, const
//char *title, const char* class);

__attribute__((unused)) void html_link_close(int sockfd);

char *text_creator(const char *message);
char *form_creator(const char *action, char *value, char *id);

#ifdef __cplusplus
}
#endif

#endif  // CWEBSERVER_HTML_HELPERS_H
