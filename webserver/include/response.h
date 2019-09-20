#ifndef RESPONSE_H
#define RESPONSE_H
#include <time.h>
#include <stdio.h>

int define_content();
char get_server_time();
int send_response(int code, Client *client);

#endif