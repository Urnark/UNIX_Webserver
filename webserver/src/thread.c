#include "../include/thread.h"

void init_threads(int capacity)
{
	create_array(&array, sizeof(thread_info), capacity);
}

int new_thread(void*(*func)(void*))
{
	thread_info ti;
	ti.id = array.size;
	array_push_item(&array, &ti);
	printf("Array size: %d, capacity: %d, s: %d\n", array.size, array.capacity, array.item_size);
	return pthread_create(&((thread_info*)array.arr[array.size - 1].element)->thread, NULL, func,(void*) ti.id);
}

void terminate_threads()
{
	for (int i = 0; i < array.size; i++)
	{
		pthread_join(((thread_info*)array.arr[i].element)->thread, NULL);
	}
	delete_array(&array);
}