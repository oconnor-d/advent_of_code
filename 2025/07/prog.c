#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_ROWS 250
#define MAX_COLS 250

#define SPLITTER '^'
#define BEAM_START 'S'

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Put the input into a 2D array
    bool beams[MAX_COLS] = {false};
    int numSplits = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        for (int idx = 0; idx < lineLen - 1; idx += 1) {
            if (line[idx] == BEAM_START) beams[idx] = true;

            if (line[idx] == SPLITTER && beams[idx]) {
                numSplits += 1;

                beams[idx] = false;
                beams[idx - 1] = true;
                beams[idx + 1] = true;
            }
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", numSplits, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Like part 1, but accumulate the counts of unique beams sent for each level
    instead of just tracking if a beam exists there or not. The total number of beams
    at the last level is the number of time splits.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    long counts[MAX_COLS] = {0};
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        for (int idx = 0; idx < lineLen - 1; idx += 1) {
            if (line[idx] == BEAM_START) counts[idx] = 1;

            if (line[idx] == SPLITTER && counts[idx]) {
                counts[idx - 1] += counts[idx];
                counts[idx + 1] += counts[idx];
                counts[idx] = 0;
            }
        }
    }

    // Get the total number of timelines (the total number of different beams making it to the final level).
    long total = 0;
    for (int c = 0; c < MAX_COLS; c += 1) total += counts[c];

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %ld [%.2fms]\n", total, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
