#include <stdio.h>


void print2DIntArray(int* array, int rows, int cols) {
    for (int r = 0; r < rows; r += 1) {
        for (int c = 0; c < cols; c += 1) {
            printf("%d, ", array[r * cols + c]);
        }
        printf("\n");
    }
}