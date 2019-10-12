#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Each element in the array.
typedef struct _array_element
{
    void* element;
} _array_Element;


// The array instance struct
typedef struct array
{
    _array_Element* arr;
    size_t item_size;
    size_t size;
    size_t capacity;
} Array;

/**
 * Creates a new array.
 * 
 * array: Array instance that should be initilized.
 * item_size: Each elements size in bytes.
 * capacity: How many elements it should allocate space for.
 * */
void array_create_array(Array* array, size_t item_size, size_t capacity);

/**
 * Delete existing array.
 * 
 * array: Array instance that should be deleted.
 * */
void array_delete_array(Array* array);

/**
 * Internal function! Check if an array's capacity should be expanded. If it is, then double the capacity of the array.
 * 
 * array: Array instance that should expand.
 * */
void _array_check_expand(Array* array);

/**
 * Add a new element at the end of the array.
 * 
 * array: Array instance that should be used.
 * value: The new element.
 * */
void array_push_element(Array* array, void* value);

/**
 * Remove an existing element from the array.
 * 
 * array: Array instance that should be deleted.
 * index: The requested element's index in the array.
 * */
void array_remove_element(Array* array, size_t index);

/**
 * Return an element in the array as a void pointer.
 * 
 * array: Array instance that should be deleted.
 * index: The requested element's index in the array.
 * 
 * Returns: The requested element as a void pointer.
 * */
void* array_get(Array* array, size_t index);

#endif