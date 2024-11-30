#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

int OPERATIONAL = 0;
int DAMAGED = 1;
int UNKNOWN = 2;


void problem1() {
    clock_t start = clock();
    
    FILE* inputFile = fopen("2023/12/input_test.txt", "r");

    int totalArrangements = 0;

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLength;


    int idx, startIdx, endIdx;
    IntArray springConditions, contiguousGroups, proposedConfiguration;
    initIntArray(&springConditions, 32);
    initIntArray(&contiguousGroups, 32);
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        idx = 0;

        // Read the spring conditions in. (Converts from char -> int since IntArray is nicer to work with than StringArray).
        while (line[idx] != ' ') {
            if (line[idx] == '.') insertIntArray(&springConditions, OPERATIONAL);
            else if (line[idx] == '#') insertIntArray(&springConditions, DAMAGED);
            else insertIntArray(&springConditions, UNKNOWN);

            idx += 1;
        }

        printIntArray(&springConditions);

        // Read the contiguous groups in.
        while (idx < lineLength - 1) {
            insertIntArray(&contiguousGroups, parseFirstNumber(line + idx, &startIdx, &endIdx));
            idx += endIdx - startIdx + 1;
        }

        printIntArray(&contiguousGroups);
        printf("\n");

        
        isValidConfiguration(&springConditions, &proposedConfiguration, &contiguousGroups);

        springConditions.numItems = 0;
        contiguousGroups.numItems = 0;
    }

    freeIntArray(&contiguousGroups);
    freeIntArray(&springConditions);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", totalArrangements, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}



int main() {
    problem1();

    return 0;
}
