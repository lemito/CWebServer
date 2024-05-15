#ifndef CWEBSERVER_LOGGER_H
#define CWEBSERVER_LOGGER_H

#include <stdio.h>
#include <time.h>

extern FILE *log_file;

void log_write(char* msg);
FILE *log_file_init(void);

#endif //CWEBSERVER_LOGGER_H
