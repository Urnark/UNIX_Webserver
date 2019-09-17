#include "../include/array.h"

/**
 * Creates a new array.
 * 
 * array: Array instance that should be initilized.
 * item_size: Each elements size in bytes.
 * capacity: How many elements it should allocate space for.
 * */
void create_array(Array* array, size_t item_size, size_t capacity)
{
    array->item_size = item_size;
    array->size = 0;
    array->capacity = capacity;
    array->arr = malloc(sizeof(Element) * array->capacity);
}

/**
 * Delete existing array.
 * 
 * array: Array instance that should be deleted.
 * */
void delete_array(Array* array)
{
    for (int i = 0; i < array->size; i++)
    {
        free(array->arr[i].element);
    }
    free(array->arr);
}

/**
 * Internal function! Check if an array's capacity should be expanded. If it is double the capacity of the array.
 * 
 * array: Array instance that should be initilized.
 * */
void _check_expand(Array* array)
{
    if (array->size == array->capacity)
    {
        array->capacity *= 2;
        array->arr = (Element*)realloc(array->arr, array->capacity *sizeof(Element));
    }
}

/**
 * Add a new element at the end of the array.
 * 
 * array: Array instance that should be deleted.
 * value: The new element.
 * */
void array_push_element(Array* array, void* value)
{
    _check_expand(array);

    array->arr[array->size].element = malloc(array->item_size);
    memcpy(array->arr[array->size].element, value, array->item_size);
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
        memcpy(array->arr[i - 1].element, array->arr[i].element, array->item_size);
    }

    array->size--;
    free(array->arr[array->size].element);
}

/**
 * Return an element in the array as a void pointer.
 * 
 * array: Array instance that should be deleted.
 * index: The requested element's index in the array.
 * 
 * Returns: The requested element as a void pointer.
 * */
void* get_element(Array* array, size_t index)
{
    return array->arr[index].element;
}