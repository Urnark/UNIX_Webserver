#ifndef THREAD_H
#define THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../include/array.h"

Array _array;

typedef struct thread_info{
	int id;
	pthread_t thread;
} Thread_info;

static int NEW_ID = 0;

void init_threads(int capacity);
int new_thread(void*(*func)(void*));
void terminate_threads();

pthread_mutex_t thread_mutex;

void exit_thread(int thread_id);

#endif