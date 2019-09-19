#include "../include/array.h"

/**
 * Creates a new array.
 * 
 * array: Array instance that should be initilized.
 * item_size: Each elements size in bytes.
 * capacity: How many elements it should allocate space for.
 * */
void array_create_array(Array* array, size_t item_size, size_t capacity)
{
    array->item_size = item_size;
    array->size = 0;
    array->capacity = capacity;
    array->arr = malloc(sizeof(_array_Element) * array->capacity);
}

/**
 * Delete existing array.
 * 
 * array: Array instance that should be deleted.
 * */
void array_delete_array(Array* array)
{
    free(array->arr);
}

/**
 * Internal function! Check if an array's capacity should be expanded. If it is double the capacity of the array.
 * 
 * array: Array instance that should be initilized.
 * */
void _array_check_expand(Array* array)
{
    if (array->size == array->capacity)
    {
        array->capacity *= 2;
        array->arr = (_array_Element*)realloc(array->arr, array->capacity *sizeof(_array_Element));
    }
}

/**
 * Add a new element at the end of the array.
 * 
 * array: Array instance that should be used.
 * value: The new element.
 * */
void array_push_element(Array* array, void* value)
{
    _array_check_expand(array);

    /*array->arr[array->size].element = malloc(array->item_size);
    memcpy(array->arr[array->size].element, value, array->item_size);*/
    array->arr[array->size].element = value;
    array->size++;
}

/**
 * Remove an existing element from the array.
 * 
 * array: Array instance that should be deleted.
 * index: The requested element's index in the array.
 * */
void array_remove_element(Array* array, size_t index)
{
    for (size_t i = index + 1; i < array->size; i++)
    {
        array->arr[i - 1].element = array->arr[i].element;
    }

    array->size--;
}

/**
 * Return an element in the array as a void pointer.
 * 
 * array: Array instance that should be deleted.
 * index: The requested element's index in the array.
 * 
 * Returns: The requested element as a void pointer.
 * */
void* array_get(Array* array, size_t index)
{
    return array->arr[index].element;
}