#ifndef THREAD_H
#define THREAD_H

#include "../include/socket.h"
#include "../include/threadManager.h"
#include "../include/request.h"
#include "../include/response.h"

#define START_NUM_THREADS 5

typedef struct thread_args
{
	int id;
	Client client;
} Thread_args;

void *client_thread(void *args);
void exec_threading();

#endif