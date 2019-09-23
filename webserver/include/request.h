#ifndef REQUEST_H
#define REQUEST_H

#include <linux/limits.h>

#include "../include/socket.h"

#define REQUEST_TIMEOUT_SEC 60

typedef enum request_type{
    RT_GET,
    RT_HEAD,
    RT_NONE
} Request_type;

typedef struct headers_t
{
    char* method;
    char* host;
    char* user_agent;
    char* accept;
    char* accept_language;
    char* accept_encoding;
    char* connection;
    char* referer;
} Headers;

typedef enum http_version_t {
    HTTP_0_9,
    HTTP_1_0,
    HTTP_1_1,
    HTTP_none
} HTTP_version;

typedef struct request_t
{
    HTTP_version http_version;
    int response_code;
    char path[PATH_MAX];
    Request_type type;
    Headers headers;
} Request_t;

char path_www_folder[PATH_MAX];
int request_stop_reciving_data;

void _set_path_to_www_folder();
void free_headers(Headers* headers);
void _init_headers(Headers* headers);
int _set_header(char** header, char* current_line, const char* comp_str);
Request_type _populate_headers(Headers* headers, char* current_line);
int _check_method(Request_t* request, Request_type rt, char* first_word);
int _check_http_version(Request_t* request, char* method);
int _check_uri(Request_t* request, char* method);
Request_t _process_request(char* request);
void request_init();
Request_t request_received(Client* client);

#endif