#ifndef array_h
#define array_h

#include <stdlib.h>

/*
    INT ARRAY
*/

typedef struct {
    int* data;
    size_t numItems;
    size_t size;
} IntArray;

/*
    LLONG ARRAY
*/

typedef struct {
    long long* data;
    size_t numItems;
    size_t size;
} LLongArray;

/*
    STRING ARRAY
*/
typedef struct {
    char** data;
    size_t numItems;
    size_t size;
    size_t stringMaxSize;
} StringArray;

#endif
