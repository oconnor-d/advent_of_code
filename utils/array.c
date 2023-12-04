#include <stdbool.h>
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


bool containsIntArray(IntArray* array, int item) {
    for (int idx = 0; idx < array->numItems; idx += 1) {
        if (array->data[idx] == item) {
            return true;
        }
    }

    return false;
}


int sumIntArray(IntArray* array) {
    int sum = 0;
    for (int idx = 0; idx < array->numItems; idx += 1) {
        sum += array->data[idx];
    }

    return sum;
}


void printIntArray(IntArray* array) {
    printf("[Size: %zu, Items: %zu] [", array->size, array->numItems);
    for (int idx = 0; idx < array->numItems; idx += 1) {
        if (idx == array->numItems - 1) {
            printf("%d", array->data[idx]);
        } else {
            printf("%d, ", array->data[idx]);
        }
    }
    printf("]\n");
}