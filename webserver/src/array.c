#include "../include/array.h"

/**
 * Creates a new array.
 * 
 * arr: Array instance that should be initilized.
 * item_size: Each elements size in bytes.
 * capacity: How much items it should allocate space for.
 * */
void create_array(Array* arr, size_t item_size, size_t capacity)
{
    arr->item_size = item_size;
    arr->size = 0;
    arr->capacity = capacity;
    arr->arr = malloc(sizeof(Element) * arr->capacity);
}

void delete_array(Array* arr)
{
    for (int i = 0; i < arr->size; i++)
    {
        free(arr->arr[i].element);
    }
    free(arr->arr);
}


// Error when expanding the array
void _check_expand(Array* arr)
{
    if (arr->size > arr->capacity)
    {
        printf("Expand Array\n");
        const int totalSize = arr->capacity + 2;
        arr->arr = (Element*)realloc(arr->arr, (totalSize *sizeof(Element)));
    }
}

void array_push_item(Array* arr, void* value)
{
    _check_expand(arr);

    arr->arr[arr->size].element = malloc(arr->item_size);
    memcpy(arr->arr[arr->size].element, value, arr->item_size);
    arr->size++;
}

void* array_remove_item(Array* arr, size_t index)
{
    void* value = arr->arr[index].element;
    for (size_t i = index + 1; i < arr->size; i++)
    {
        memcpy(arr->arr[i - 1].element, arr->arr[i].element, arr->item_size);
    }

    arr->size--;
    free(arr->arr[arr->size].element);

    return value;
}