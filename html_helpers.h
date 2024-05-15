//
// Created by lemito on 5/15/24.
//

#ifndef CWEBSERVER_HTML_HELPERS_H
#define CWEBSERVER_HTML_HELPERS_H

#include <stddef.h>
#include "handlers.h"
#include "file_handling.h"

void html_raw(int sockfd, const char* data, size_t size);
void html(int sockfd, const char *txt);
void html_attr(int sockfd, const char *txt);
void html_link_open(int sockfd, const char *url, const char *title, const char *class);
void html_link_close(int sockfd);

char *text_creator(const char *message);
char *form_creator(const char *action, char *value, char *id);


void router(char *route, int sockfd, char *buffer);

#endif //CWEBSERVER_HTML_HELPERS_H
