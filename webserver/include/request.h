#ifndef REQUEST_H
#define REWUEST_H

#include <linux/limits.h>

#include "../include/socket.h"

typedef enum {
    RT_GET,
    RT_HEAD,
    RT_NONE
} Request_type;

char path_www_folder[PATH_MAX];

void set_path_to_www_folder();
int request_recived(Client* client);

#endif