#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <stdio.h>

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

void create_array(Array* arr, size_t item_size, size_t capacity);
void delete_array(Array* arr);
void _check_expand(Array* arr);
void array_push_item(Array* arr, void* value);
void* array_remove_item(Array* arr, size_t index);

#endif