//
// Created by lemito on 5/15/24.
//

#ifndef CWEBSERVER_CONFIG_H
#define CWEBSERVER_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif


// standard includes
#ifdef _POSIX_C_SOURCE
#include <sys/socket.h>
#include <unistd.h>
#elif defined(WIN32)
#include <winsock2.h>
#endif

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
typedef enum HTTP_METHODS {
    GET,
    POST,
    HttpDelete,
    PUT
} HTTP_METHODS;

#ifdef __cplusplus
}
#endif

#endif //CWEBSERVER_CONFIG_H
