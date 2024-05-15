#ifndef CWEBSERVER_MAIN_H
#define CWEBSERVER_MAIN_H

// standard includes
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// my includes
//#include "responses.h"

// constants
#define PORT 8080
#define BUFFER_SIZE 4096
#define HTTP_OK 200
#define HTTP_NOT_FOUND 404
#define HTTP_BAD_REQUEST 400
#define HTTP_NOT_ALLOWED 405

#define STATUS_TEXT_OK "OK"
#define STATUS_TEXT_NOT_FOUND "Not Found"
#define STATUS_TEXT_BAD_REQUEST "Bad Request"
#define STATUS_TEXT_NOT_ALLOWED "Method Not Allowed"

#define JPEG "image/jpeg"
#define JS "text/javascript"
#define CSS "text/css"

// macros
#define FREE_AND_NULL(p) \
    do                   \
    {                    \
        free(p);         \
        (p) = NULL;      \
    } while (0)

// enums
typedef enum {
    GET,
    POST,
    DELETE,
    PUT
} HTTP_METHODS;

// global variables

extern volatile sig_atomic_t server_running;

// function prototypes
char *parse_url(char *buffer);

void html_raw(int sockfd, const char* data, size_t size);
void html(int sockfd, const char *txt);
void html_attr(int sockfd, const char *txt);
void html_link_open(int sockfd, const char *url, const char *title, const char *class);
void html_link_close(int sockfd);

char *text_creator(const char *message);
char *form_creator(const char *action, char *const value, char *const id);
int setup_socket(void);
void setup_server(int sockfd);
char *read_all_from_html(FILE *fd);
void send_static_file(int sockfd, const char *filename);
void router(char *route, int sockfd, char *buffer);
void handle_signal(int sig);

#endif //CWEBSERVER_MAIN_H
