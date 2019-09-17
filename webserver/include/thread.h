#ifndef THREAD_H
#define THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../include/array.h"

Array array;

typedef struct {
	int id;
	pthread_t thread;
} thread_info;

void init_threads(int capacity);
int new_thread(void*(*func)(void*));
void terminate_threads();

#endif