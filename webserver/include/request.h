#ifndef REQUEST_H
#define REQUEST_H

#include <linux/limits.h>

#include "../include/socket.h"

typedef enum {
    RT_GET,
    RT_HEAD,
    RT_NONE
} Request_type;

typedef struct request_t
{
    int simple;
    int response_code;
    char path[PATH_MAX];
    Request_type type;
} Request_t;

char path_www_folder[PATH_MAX];
int request_stop_reciving_data;

void _set_path_to_www_folder();
void request_init();
Request_t request_recived(Client* client);
Request_t _process_request(char* request);

#endif