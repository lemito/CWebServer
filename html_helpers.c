//
// Created by lemito on 5/15/24.
//

#include "html_helpers.h"


char *text_creator(const char *message)
{
    size_t message_len = strlen(message);
    size_t response_len = message_len + 8;
    char *response = malloc(response_len);
    if (response == NULL)
    {
        fprintf(stderr, "Не удалось создать response");
        return NULL;
    }

    snprintf(response, response_len, "<p>%s</p>", message);
    return response;
}

char *form_creator(const char *action, char *const value, char *const id)
{
    char *response = malloc(1024);

    snprintf(response, 1024, "<form action='%s' method='post'><input value='%s' id='%s' type='text' name='name' /><button type='submit'>Submit</button></form>", action, value, id);
    return response;
}



void html_raw(int sockfd, const char* data, size_t size){
    if (write(sockfd, data, size) < 0)
        fprintf(stderr, "Ошибка записи html вывода");
}

void html(int sockfd, const char *txt)
{
    size_t txt_len = strlen(txt);
    html_raw(sockfd, txt, txt_len);
}

void html_attr(int sockfd, const char *txt)
{
    const char *t = txt;
    while (t && *t) {
        int c = (int)*t;
        if (c == '<' || c == '>' || c == '\'' || c == '\"' || c == '&') {
            html_raw(sockfd, txt, t - txt);
            if (c == '>')
                html(sockfd, "&gt;");
            else if (c == '<')
                html(sockfd, "&lt;");
            else if (c == '\'')
                html(sockfd, "&#x27;");
            else if (c == '"')
                html(sockfd, "&quot;");
            else html(sockfd, "&amp;");
            txt = t + 1;
        }
        t++;
    }
    if (t != txt)
        html(sockfd, txt);
}

__attribute__((unused)) void html_link_open(int sockfd, const char *url, const char *title, const char *class)
{
    html(sockfd, "<a href='");
    html_attr(sockfd, url);
    if (title) {
        html(sockfd, "' title='");
        html_attr(sockfd, title);
    }
    if (class) {
        html(sockfd, "' class='");
        html_attr(sockfd, class);
    }
    html(sockfd, "'>");
}

__attribute__((unused)) void html_link_close(int sockfd)
{
    html(sockfd, "</a>");
}