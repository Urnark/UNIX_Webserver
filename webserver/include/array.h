#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    void* element;
} Element;


typedef struct
{
    Element* arr;
    size_t item_size;
    size_t size;
    size_t capacity;
} Array;

void create_array(Array* ararrayr, size_t item_size, size_t capacity);
void delete_array(Array* array);
void _check_expand(Array* array);
void array_push_element(Array* array, void* value);
void array_remove_element(Array* array, size_t index);
void* get_element(Array* array, size_t index);

/**
 * Return an element in the array.
 * 
 * array: Array instance that should be deleted.
 * index: The requested element's index in the array.
 * 
 * Returns: The requested element.
 * */
#define get(type, array, index) *(type*)get_element(&array, index)

#endif