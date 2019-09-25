#ifndef RESPONSE_H
#define RESPONSE_H
#include <time.h>
#include <stdio.h>
#include "socket.h"
#include "request.h"
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

char* get_server_time(char* time_string);
char* define_content(Request_t* request);
char* write_head(Request_t* request, Client *client);
int send_response(Request_t* request, Client *client);

#endif