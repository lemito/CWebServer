#ifndef CWEBSERVER_LOGGER_H
#define CWEBSERVER_LOGGER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <time.h>

extern FILE *log_file;

void log_write(char* msg);
FILE *log_file_init(void);

#ifdef __cplusplus
}
#endif



#endif //CWEBSERVER_LOGGER_H
