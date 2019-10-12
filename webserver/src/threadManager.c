#include "../include/threadManager.h"

/**
 * Initilize the thread manager.
 * 
 * capacity: The expected number of threads in the thread manager.
 * */
void thread_manager_init_threads(int capacity)
{
	array_create_array(&_thread_manager_array, sizeof(_thread_manager_Thread_info), capacity);
	pthread_mutex_init(&_thread_manager_thread_mutex, NULL);
}

/**
 * Add a new thread to the thread manager.
 * 
 * func: Function that the thread shall use.
 * args: The arguments that is passed to the function, the first argument is allways the ID(int) of the thread. If NULL only the thread ID is passed as an argument to the function.
 * 
 * Returns: If the thread was successfully created.
 * */
int thread_manager_new_thread(void*(*func)(void*), void* args)
{
	// Lock the mutex to be able to make changes to the array
	pthread_mutex_lock(&_thread_manager_thread_mutex);

	_thread_manager_Thread_info* ti = (_thread_manager_Thread_info*)malloc(sizeof(_thread_manager_Thread_info));
	ti->id = _thread_manager_NEW_ID++;
	array_push_element(&_thread_manager_array, (void*)ti);
	
	_thread_manager_Thread_info* thread_i = (_thread_manager_Thread_info*)array_get(&_thread_manager_array, _thread_manager_array.size - 1);
	int ret = 0;
	if (args != NULL)
	{
		(*(int*)args) = thread_i->id;
		ret = pthread_create(&(thread_i->thread), NULL, func, args);
	}
	else
	{
		ret = pthread_create(&(thread_i->thread), NULL, func,(void*) &(thread_i->id));
	}

	pthread_mutex_unlock(&_thread_manager_thread_mutex);
	return ret;
}

/**
 * Terminate the all the threads.
 * */
void thread_manager_terminate_threads()
{
	pthread_mutex_destroy(&_thread_manager_thread_mutex);

	int i;
	for (i = 0; i < _thread_manager_array.size; i++)
	{
		pthread_join(((_thread_manager_Thread_info*)array_get(&_thread_manager_array, i))->thread, NULL);
	}

	// Because the Thread_info is allocated on the heap, free needs to be called
	for (i = 0; i < _thread_manager_array.size; i++)
    {
        free(array_get(&_thread_manager_array, i));
    }
	// Deallocate the array
	array_delete_array(&_thread_manager_array);
}

/**
 * Telling the thread manager the thread that called the function is done, so deallocate the internal memory that was allocated for it.
 * */
void thread_manager_exit_thread(void* args)
{
	int thread_id = *(int*)args;
	pthread_mutex_lock(&_thread_manager_thread_mutex);

	int i;
	for (i = 0; i < _thread_manager_array.size; i++)
	{
		if (((_thread_manager_Thread_info*)array_get(&_thread_manager_array, i))->id == thread_id)
		{
			free(((_thread_manager_Thread_info*)array_get(&_thread_manager_array, i)));
			array_remove_element(&_thread_manager_array, i);
			break;
		}
	}

	pthread_mutex_unlock(&_thread_manager_thread_mutex);
	free(args);
}