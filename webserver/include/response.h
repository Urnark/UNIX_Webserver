#ifndef RESPONSE_H
#define RESPONSE_H
#include <time.h>
#include <stdio.h>
#include "socket.h"
#include "request.h"

char* define_content(Request_t* request);
char* get_server_time();
int send_response(Request_t* request, Client *client);

#endif