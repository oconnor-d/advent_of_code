#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "../../utils/array.c"


long long manhattanDistance(long long x1, long long y1, long long x2, long long y2) {
    return llabs(x1 - x2) + llabs(y1 - y2);
}


void problem(int problemNumber, int galaxyExpandBy) {
    clock_t start = clock();
    
    FILE* inputFile = fopen("2023/11/input.txt", "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLength;

    int numRows = 0, numCols = 0;
    IntArray emptyRows, emptyCols;
    initIntArray(&emptyRows, 32);
    initIntArray(&emptyCols, 32);

    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        numCols = lineLength;
        numRows += 1;
    }
    fseek(inputFile, 0, SEEK_SET);

    LLongArray galaxyCoordinates;
    initLLongArray(&galaxyCoordinates, 32);
    char universe[numRows][numCols];
    int row = 0;
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        for (int col = 0; col < lineLength; col += 1) {
            universe[row][col] = line[col];
        }
        row += 1;
    }

    // Get the empty rows
    bool rowEmpty;
    for (int row = 0; row < numRows; row += 1) {
        rowEmpty = true;
        for (int col = 0; col < numCols; col += 1) {
            if (universe[row][col] != '.') {
                rowEmpty = false;
                break;
            }
        }

        if (rowEmpty) insertIntArray(&emptyRows, row);
    }

    // Get the empty cols
    bool colEmpty;
    for (int col = 0; col < numCols; col += 1) {
        colEmpty = true;
        for (int row = 0; row < numRows; row += 1) {
            if (universe[row][col] != '.') {
                colEmpty = false;
                break;
            }
        }

        if (colEmpty) insertIntArray(&emptyCols, col);
    }

    // Get the galaxy coordinates, with the extra space already baked in.
    long long rowOffset = 0, colOffset = 0;;
    for (int row = 0; row < numRows; row += 1) {
        if (containsIntArray(&emptyRows, row)) {
            rowOffset += galaxyExpandBy;
            continue;
        }

        colOffset = 0;
        for (int col = 0; col < numCols; col += 1) {
            if (containsIntArray(&emptyCols, col)) {
                colOffset += galaxyExpandBy;
                continue;
            }

            if (universe[row][col] == '#') {
                insertLLongArray(&galaxyCoordinates, row + rowOffset);
                insertLLongArray(&galaxyCoordinates, col + colOffset);
            }
        }
    }

    freeIntArray(&emptyRows);
    freeIntArray(&emptyCols);

    long long shortestDistanceSum = 0;
    long long x1, y1, x2, y2;
    // Don't need to consider the last coords, since they would already be paired with
    // every earlier galaxy by the point we get to it.
    for (int idx = 0; idx < galaxyCoordinates.numItems - 2; idx += 2) {
        x1 = galaxyCoordinates.data[idx];
        y1 = galaxyCoordinates.data[idx + 1];

        for (int pairIdx = idx + 2; pairIdx < galaxyCoordinates.numItems; pairIdx += 2) {
            x2 = galaxyCoordinates.data[pairIdx];
            y2 = galaxyCoordinates.data[pairIdx + 1];

            shortestDistanceSum += manhattanDistance(x1, y1, x2, y2);
        }
    }

    freeLLongArray(&galaxyCoordinates);

    clock_t end = clock();
    printf("Problem 0%d: %lld [%.2fms]\n", problemNumber, shortestDistanceSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}



int main() {
    problem(1, 1);
    problem(2, 999999);

    return 0;
}
