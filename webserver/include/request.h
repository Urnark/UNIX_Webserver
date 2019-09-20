#ifndef REQUEST_H
#define REWUEST_H

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
    int result_code;
    char path[PATH_MAX];
    Request_type type;
} Request_t;

char path_www_folder[PATH_MAX];

void set_path_to_www_folder();
Request_t request_recived(Client* client);

#endif