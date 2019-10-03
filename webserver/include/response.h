#ifndef RESPONSE_H
#define RESPONSE_H
#include <time.h>
#include <stdio.h>
#include "socket.h"
#include "request.h"
#include "read_config.h"
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

typedef struct _head
{
    char *http_version;
    char *code;
    char *code_notice;
    char *server_time;
    char *name;
    char *server_version;
    char *content_type;
    int content_size;
    char *connection_type;
    char *client_ip;
}HTTP_HEAD;

typedef struct _file
{
    char* file_content;
    int length;
} MyFile;


int response_malloc_head(HTTP_HEAD response_head);
int response_free_head(HTTP_HEAD respnse_head);
char* get_server_time(char* time_string);
MyFile* define_content(Request_t* request);
void add_log(HTTP_HEAD* response_head, Request_t* request, int size);
int send_response(Client *client, char *response, int response_size);
int build_response(HTTP_HEAD response_head, Request_t* request, Client *client, int head_true, int content_true);
int gather_response_information(Request_t* request, Client *client);

#endif