#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct _array_element
{
    void* element;
} _array_Element;


typedef struct array
{
    _array_Element* arr;
    size_t item_size;
    size_t size;
    size_t capacity;
} Array;

void array_create_array(Array* array, size_t item_size, size_t capacity);
void array_delete_array(Array* array);
void _array_check_expand(Array* array);
void array_push_element(Array* array, void* value);
void array_remove_element(Array* array, size_t index);
void* array_get(Array* array, size_t index);

#endif