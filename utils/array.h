#ifndef array_h
#define array_h

#include <stdlib.h>


typedef struct {
    /* data */
    int* data;
    size_t numItems;
    size_t size;
} IntArray;


void initIntArray(IntArray* array, size_t initialSize);
void freeIntArray(IntArray* array);
void insertIntArray(IntArray* array, int item);

#endif