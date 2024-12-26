#include <stdbool.h>
#include <stdlib.h>

/*
An IntArraySet is a non-hashing set implementation of integers. The set must be of a known max-size.

There's currently no way to iterate over just the elements in the list, you must iterate over
all possible values and check if the value is actually contained in the list.
*/
typedef struct {
    bool* data;
    size_t size;
    size_t numTrue;
} IntArraySet;

void initIntArraySet(IntArraySet* set, size_t size) {
    set->data = malloc(size * sizeof(bool));
    for (int idx = 0; idx < size; idx += 1) set->data[idx] = false;
    set->size = size;
    set->numTrue = 0;
}

void freeIntArraySet(IntArraySet* set) {
    free(set->data);
    set->size = 0;
    set->numTrue = 0;
}

bool addIntArraySet(IntArraySet* set, int item) {
    if (item >= set->size) return false;

    if (!set->data[item]) {
        set->data[item] = true;
        set->numTrue += 1;
    }

    return true;
}

bool removeIntArraySet(IntArraySet* set, int item) {
    if (item >= set->size) return false;

    if (set->data[item]) {
        set->data[item] = false;
        set->numTrue -= 1;
    }

    return true;
}

void unionIntArraySet(IntArraySet* a, IntArraySet* b, IntArraySet* out) {
    /*
    Stores the union of `a` and `b` in `out`.

    `out` is initialized to the max size of `a` and `b`
    */
    initIntArraySet(out, a->size > b->size ? a->size : b->size);

    for (int aIdx = 0; aIdx < a->size; aIdx += 1) {
        if (a->data[aIdx]) addIntArraySet(out, aIdx);
    }

    for (int bIdx = 0; bIdx < b->size; bIdx += 1) {
        if (b->data[bIdx]) addIntArraySet(out, bIdx);
    }
}

void intersectionIntArraySet(IntArraySet* a, IntArraySet* b, IntArraySet* out) {
    /*
    Stores the intersection of `a` and `b` in `out`.

    `out` is initialized to the max size of `a` and `b`.
    */
    int greaterSize = a->size > b->size ? a->size : b->size;
    int lesserSize = a->size < b->size ? a->size : b->size;
    initIntArraySet(out, greaterSize);

    for (int idx = 0; idx < lesserSize; idx += 1) {
        if (a->data[idx] && b->data[idx]) addIntArraySet(out, idx);
    }
}

bool containsIntArraySet(IntArraySet* set, int value) {
    if (value >= set->size) return false;

    return set->data[value];
}

void printIntArraySet(IntArraySet* set) {
    printf("[NumTrue: %zu] {", set->numTrue);
    for (int idx = 0; idx < set->size; idx += 1) {
        if (!set->data[idx]) continue;
        printf("%d ", idx);
    }
    printf("}\n");
}
