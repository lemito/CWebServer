//
// Created by lemito on 5/15/24.
//

#ifndef CWEBSERVER_FILE_HANDLING_H
#define CWEBSERVER_FILE_HANDLING_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "responses.h"
#include "config.h"


char *read_all_from_html(FILE *fd);
void send_static_file(int sockfd, const char *filename);

#ifdef __cplusplus
}
#endif



#endif //CWEBSERVER_FILE_HANDLING_H
