#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../../utils/string.c"


int max(int a, int b) {
    return (a > b) ? a : b;
}


void problem_1_and_2() {
    clock_t start = clock();

    int RED_TARGET = 12, GREEN_TARGET = 13, BLUE_TARGET = 14;

    FILE* gameFile = fopen("2023/02/input.txt", "r");

    int gameIdSum = 0, cubePowerSum = 0;

    char *line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int gameId = 0;
    while ((lineLen = getline(&line, &lineCap, gameFile)) > 0) {
        int maxRed = -1, maxBlue = -1, maxGreen = -1;
        gameId += 1;

        // The first number alway starts at this idx.
        int idx = 8;
        while (idx < lineLen) {
            int numberStartIdx, numberEndIdx;
            int number = parseFirstNumber(line + idx, &numberStartIdx, &numberEndIdx);

            // The color starts two characters after the number ends.
            idx += numberEndIdx + 1;
            if (line[idx] == 'r') {
                maxRed = max(maxRed, number);
                idx += 5;
            } else if (line[idx] == 'g') {
                maxGreen = max(maxGreen, number);
                idx += 7;
            } else {
                // must be blue
                maxBlue = max(maxBlue, number);
                idx += 6;
            }
        }

        if (maxRed <= RED_TARGET && maxGreen <= GREEN_TARGET && maxBlue <= BLUE_TARGET) {
            gameIdSum += gameId;
        }

        cubePowerSum += (maxRed * maxGreen * maxBlue);
    }

    clock_t end = clock();
    double timeElapsedInMilliSeconds = (double)(end - start) / CLOCKS_PER_SEC * 1000;

    printf("Problem 01: %d [%.2fms]\n", gameIdSum, timeElapsedInMilliSeconds);
    printf("Problem 02: %d [%.2fms]\n", cubePowerSum, timeElapsedInMilliSeconds);
}

int main() {
    problem_1_and_2();

    return 0;
}