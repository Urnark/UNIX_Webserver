#ifndef THREAD_H
#define THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../include/array.h"

Array _thread_manager_array;
pthread_mutex_t _thread_manager_thread_mutex;

static int _thread_manager_NEW_ID = 0;

// Internal struct! Holds a threads ID and the struct to the thread indentifiers
typedef struct _thread_manager_thread_info{
	int id;
	pthread_t thread;
} _thread_manager_Thread_info;

/**
 * Initilize the thread manager.
 * 
 * capacity: The expected number of threads in the thread manager.
 * */
void thread_manager_init_threads(int capacity);

/**
 * Add a new thread to the thread manager.
 * 
 * func: Function that the thread shall use.
 * args: The arguments that is passed to the function, the first argument is allways the ID(int) of the thread. If NULL only the thread ID is passed as an argument to the function.
 * 
 * Returns: If the thread was successfully created.
 * */
int thread_manager_new_thread(void*(*func)(void*), void* args);

/**
 * Terminate the all the threads.
 * */
void thread_manager_terminate_threads();

/**
 * Telling the thread manager the thread that called the function is done, so deallocate the internal memory that was allocated for it.
 * */
void thread_manager_exit_thread(void* args);

#endif