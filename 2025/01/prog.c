#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int dialPosition = 50;
    int zeroesEncountered = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        if (line[0] == 'L') {
            dialPosition = (dialPosition - atoi(line + 1)) % 100;
        } else {
            dialPosition = (dialPosition + atoi(line + 1)) % 100;
        }

        if (dialPosition == 0) zeroesEncountered += 1;
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", zeroesEncountered, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int startPosition = 0;
    int dialPosition = 50;
    int zeroesEncountered = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        startPosition = dialPosition;
        if (line[0] == 'L') {
            dialPosition = dialPosition - atoi(line + 1);
        } else {
            dialPosition = dialPosition + atoi(line + 1);
        }

        if (dialPosition > 99) {
            zeroesEncountered += dialPosition / 100;

            dialPosition = dialPosition % 100;
        } else if (dialPosition < 0) {
            zeroesEncountered += (-1 * dialPosition / 100) + (startPosition == 0 ? 0 : 1);

            dialPosition = 100 + (dialPosition % 100);
            if (dialPosition == 100) dialPosition = 0;
        } else if (dialPosition == 0) {
            zeroesEncountered += 1;
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", zeroesEncountered, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
