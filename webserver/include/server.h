#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <unistd.h>

#include "../include/threadManager.h"
#include "../include/socket.h"
#include "../include/request.h"
#include "../include/response.h"
#include "../include/logging.h"

#define START_NUM_THREADS 5

typedef struct thread_args
{
	int id;
	Client client;
    int is_fork;
} Thread_args;


#endif