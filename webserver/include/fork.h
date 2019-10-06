#ifndef FORK_H
#define FORK_H

#include <sys/types.h>
#include <unistd.h>

#include "../include/socket.h"
#include "../include/request.h"
#include "../include/response.h"
#include "../include/array.h"

Array fork_array;

typedef struct fork_array_info
{
	int id;
	pid_t p;
    int free_fork;
    int kill_fork;
} Fork_array_info;

typedef struct fork_args
{
	int id;
	Client client;
} Fork_args;

void exec_preforking();

#endif