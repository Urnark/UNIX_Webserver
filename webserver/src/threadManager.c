#include "../include/threadManager.h"

void thread_manager_init_threads(int capacity)
{
	array_create_array(&_thread_manager_array, sizeof(_thread_manager_Thread_info), capacity);
	pthread_mutex_init(&_thread_manager_thread_mutex, NULL);
}

int thread_manager_new_thread(void*(*func)(void*), void* args)
{
	pthread_mutex_lock(&_thread_manager_thread_mutex);

	_thread_manager_Thread_info* ti = (_thread_manager_Thread_info*)malloc(sizeof(_thread_manager_Thread_info));
	ti->id = _thread_manager_NEW_ID++;
	array_push_element(&_thread_manager_array, (void*)ti);

	//printf("Array size: %zu, capacity: %zu, s: %zu\n", array.size, array.capacity, array.item_size);
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

void thread_manager_terminate_threads()
{
	pthread_mutex_destroy(&_thread_manager_thread_mutex);

	for (int i = 0; i < _thread_manager_array.size; i++)
	{
		pthread_join(((_thread_manager_Thread_info*)array_get(&_thread_manager_array, i))->thread, NULL);
	}

	// Because the Thread_info is allocated on the heap, free needs to be called
	for (int i = 0; i < _thread_manager_array.size; i++)
    {
        free(array_get(&_thread_manager_array, i));
    }
	// Deallocate the array
	array_delete_array(&_thread_manager_array);
}

void thread_manager_exit_thread(void* args)
{
	int thread_id = *(int*)args;
	pthread_mutex_lock(&_thread_manager_thread_mutex);

	for (int i = 0; i < _thread_manager_array.size; i++)
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
	pthread_exit(NULL);
}