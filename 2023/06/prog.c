#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "../../utils/array.c"
#include "../../utils/string.c"


int countWins(long raceTimeMs, long raceDistanceMM) {
    // Uses the quadratic formula basically.
    double discriminant = sqrt(raceTimeMs * raceTimeMs - 4 * raceDistanceMM);
    double minHoldTime = (raceTimeMs - discriminant) / 2;
    double maxHoldTime = (raceTimeMs + discriminant) / 2;

    minHoldTime = (ceil(minHoldTime) == minHoldTime) ? ceil(minHoldTime) + 1 : ceil(minHoldTime);
    maxHoldTime = (floor(maxHoldTime) == maxHoldTime) ? floor(maxHoldTime) - 1: floor(maxHoldTime);
   
    return 1 + maxHoldTime - minHoldTime;
}


void problem1() {
    clock_t start = clock();
    
    FILE* inputFile = fopen("2023/06/input.txt", "r");

    char* line = NULL;
    char* buffer;
    size_t lineCap = 0;

    IntArray raceTimesMs, raceDistancesMM;
    initIntArray(&raceTimesMs, 4);
    initIntArray(&raceDistancesMM, 4);

    // Get the race times.
    getline(&line, &lineCap, inputFile);
    buffer = line;
    int startIdx, endIdx;
    while (*buffer) {
        insertIntArray(&raceTimesMs, parseFirstNumber(buffer, &startIdx, &endIdx));
        buffer += endIdx + 1;
    }

    // Get the race distances.
    getline(&line, &lineCap, inputFile);
    buffer = line;
    while (*buffer) {
        insertIntArray(&raceDistancesMM, parseFirstNumber(buffer, &startIdx, &endIdx));
        buffer += endIdx + 1;
    }

    int totalWaysToWin = 1;
    for (int idx = 0; idx < raceTimesMs.numItems; idx += 1) {
        totalWaysToWin *= countWins(raceTimesMs.data[idx], raceDistancesMM.data[idx]);
    }

    freeIntArray(&raceDistancesMM);
    freeIntArray(&raceTimesMs);


    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", totalWaysToWin, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}


void problem2() {
    clock_t start = clock();
    
    FILE* inputFile = fopen("2023/06/input.txt", "r");

    char* line = NULL;
    size_t lineCap = 0;

    // Get the race time.
    getline(&line, &lineCap, inputFile);

    int startIdx, endIdx;
    long raceTimeMs = parseFirstNumber(stripWhitespace(line), &startIdx, &endIdx);
    
    // Get the race distance.
    getline(&line, &lineCap, inputFile);
    long raceDistanceMM = parseFirstNumber(stripWhitespace(line), &startIdx, &endIdx);

    long long raceWaysToWin = countWins(raceTimeMs, raceDistanceMM);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", raceWaysToWin, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}



int main() {
    problem1();
    problem2();

    return 0;
}