#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "../../utils/array.c"
#include "../../utils/string.c"


void problem1() {
    clock_t start = clock();
    
    FILE* inputFile = fopen("2023/09/input.txt", "r");

    long long endTotal = 0;
    long long startTotal = 0;

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLength;


    IntArray oasisReadings, firstNumbers, lastNumbers;
    int startIdx = 0, endIdx = 0;
    int prevNumber, currentNumber;
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        initIntArray(&oasisReadings, 32);
        initIntArray(&firstNumbers, 32);
        initIntArray(&lastNumbers, 32);

        int idx = 0;
        while (idx < lineLength) {
            currentNumber = parseFirstNumber(line + idx, &startIdx, &endIdx);
            idx += endIdx - startIdx + 1;

            insertIntArray(&oasisReadings, currentNumber);
        }

        bool allZeros = false;
        while (!allZeros) {
            insertIntArray(&firstNumbers, oasisReadings.data[0]);
            insertIntArray(&lastNumbers, oasisReadings.data[oasisReadings.numItems - 1]);

            prevNumber = oasisReadings.data[0];
            for (int idx = 1; idx < oasisReadings.numItems; idx += 1) {
                insertAtIntArray(&oasisReadings, oasisReadings.data[idx] - prevNumber, idx - 1);
                prevNumber = oasisReadings.data[idx];
            }

            // Each derivative list has one last item in it.
            oasisReadings.numItems -= 1;

            allZeros = true;
            for (int idx = 0; idx < oasisReadings.numItems; idx += 1) {
                allZeros = allZeros && oasisReadings.data[idx] == 0;
            }
        }

        int firstPrediction = 0, lastPrediction = 0;
        for (int idx = lastNumbers.numItems - 1; idx >= 0; idx -= 1) {
            lastPrediction = lastPrediction + lastNumbers.data[idx];
            firstPrediction = firstNumbers.data[idx] - firstPrediction;
        }

        endTotal += lastPrediction;
        startTotal += firstPrediction;

        freeIntArray(&firstNumbers);
        freeIntArray(&lastNumbers);
        freeIntArray(&oasisReadings);
    }

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", endTotal, (double)(end - start) / CLOCKS_PER_SEC * 1000);\
    printf("Problem 02: %lld [%.2fms]\n", startTotal, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}


int main() {
    problem1();

    return 0;
}
