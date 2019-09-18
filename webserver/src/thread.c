#include "../include/thread.h"

void init_threads(int capacity)
{
	create_array(&_array, sizeof(Thread_info), capacity);
	pthread_mutex_init(&thread_mutex, NULL);
}

int new_thread(void*(*func)(void*))
{
	pthread_mutex_lock(&thread_mutex);

	Thread_info* ti = (Thread_info*)malloc(sizeof(Thread_info));
	ti->id = NEW_ID++;
	array_push_element(&_array, (void*)ti);

	pthread_mutex_unlock(&thread_mutex);

	//printf("Array size: %zu, capacity: %zu, s: %zu\n", array.size, array.capacity, array.item_size);
	Thread_info* thread_i = (Thread_info*)get(&_array, _array.size - 1);
	return pthread_create(&(thread_i->thread), NULL, func,(void*) &(thread_i->id));
}

void terminate_threads()
{
	pthread_mutex_destroy(&thread_mutex);

	for (int i = 0; i < _array.size; i++)
	{
		pthread_join(((Thread_info*)get(&_array, i))->thread, NULL);
	}

	// Because the Thread_info is allocated on the heap, free needs to be called
	for (int i = 0; i < _array.size; i++)
    {
        free(get(&_array, i));
    }
	// Deallocate the array
	delete_array(&_array);
}

void exit_thread(int thread_id)
{
	pthread_mutex_lock(&thread_mutex);

	for (int i = 0; i < _array.size; i++)
	{
		if (((Thread_info*)get(&_array, i))->id == thread_id)
		{
			array_remove_element(&_array, i);
			break;
		}
	}

	pthread_mutex_unlock(&thread_mutex);
	pthread_exit(NULL);
}