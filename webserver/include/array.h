#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct element
{
    void* element;
} Element;


typedef struct array
{
    Element* arr;
    size_t item_size;
    size_t size;
    size_t capacity;
} Array;

void create_array(Array* array, size_t item_size, size_t capacity);
void delete_array(Array* array);
void _check_expand(Array* array);
void array_push_element(Array* array, void* value);
void array_remove_element(Array* array, size_t index);
void* get(Array* array, size_t index);

#endif