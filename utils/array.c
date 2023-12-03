#include <stdlib.h>

#include "array.h"


void initIntArray(IntArray* array, size_t initialSize) {
    array->data = malloc(initialSize * sizeof(int));
    array->numItems = 0;
    array->size = initialSize;
}


void freeIntArray(IntArray* array) {
    free(array->data);
    array->data = NULL;
    array->numItems = 0;
    array->size = 0;
}


void insertIntArray(IntArray* array, int item) {
    if (array->numItems >= array->size) {
        // Grow the array.
        array->size = array->size * 2;
        array->data = realloc(array->data, array->size * sizeof(int));
    }

    array->data[array->numItems] = item;
    array->numItems += 1;
}