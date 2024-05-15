//
// Created by lemito on 5/15/24.
//

#ifndef CWEBSERVER_CONFIG_H
#define CWEBSERVER_CONFIG_H

// standard includes
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

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

#endif //CWEBSERVER_CONFIG_H
