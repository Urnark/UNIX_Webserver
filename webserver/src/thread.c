#include "../include/thread.h"

void init_threads(int capacity)
{
	create_array(&array, sizeof(thread_info), capacity);
}

int new_thread(void*(*func)(void*))
{
	thread_info ti;
	ti.id = array.size;
	array_push_element(&array, (void*)&ti);
	//printf("Array size: %zu, capacity: %zu, s: %zu\n", array.size, array.capacity, array.item_size);
	thread_info* thread_i = &get(thread_info, array, array.size - 1);
	return pthread_create(&(thread_i->thread), NULL, func,(void*) &(thread_i->id));
}

void terminate_threads()
{
	for (int i = 0; i < array.size; i++)
	{
		pthread_join(((thread_info*)array.arr[i].element)->thread, NULL);
	}
	delete_array(&array);
}