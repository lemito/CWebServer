#include "logger.h"

FILE *log_file_init()
{
    time_t rawtime;
    struct tm *timeinfo;
    char filename[45];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(filename, "LOG-%d-%02d-%02d_%02d-%02d-%02d.txt",
            1900 + timeinfo->tm_year, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Ошибка открытия файла.\n");
        return (FILE *)NULL;
    }

    printf("Лог-файл с именем %s успешно создан.\n", filename);

    return file;
}

void log_write(char* msg){
    if (log_file != NULL) {
        fprintf(log_file, "%s\n", msg);
        fprintf(stdout, "%s\n", msg);
        fflush(log_file);
        fflush(stdout);
    }
    else {
        perror("Файла нет!");
        return;
    }
}