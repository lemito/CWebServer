//
// Created by lemito on 5/15/24.
//
#include "utils.h"

char *parse_url(char *buffer) {
  // GET /122 HTTP/1.1
  //    |    |
  // нам нужно забрать всё, что между палочками -- это url
  // первый пробел
  char *method_end = strchr(buffer, ' ');
  if (!method_end) {
    fprintf(stderr, "Неверный формат запроса\n");
    return NULL;
  }

  // "/" ~= начало url
  char *url_start = method_end + 1;

  // второй пробел
  char *url_end = strchr(url_start, ' ');
  if (!url_end) {
    fprintf(stderr, "Неверный формат запроса\n");
    return NULL;
  }

  // длина url
  size_t url_length = url_end - url_start;

  // создаем строку для url
  char *url = malloc(url_length + 1);
  if (!url) {
    fprintf(stderr, "Не удалось выделить память\n");
    return NULL;
  }
  strncpy(url, url_start, url_length);
  url[url_length] = '\0';

  return url;
}