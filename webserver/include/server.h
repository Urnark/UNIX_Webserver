#ifndef SERVER_H
#define SERVER_H

#include "../include/logging.h"
#include "../include/socket.h"
#include "../include/thread.h"
#include "../include/fork.h"
#include "../include/request.h"

void start_server(int port, int log, int deamon, int setting);

#endif