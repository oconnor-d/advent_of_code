#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"

#define MAX_ROWS 1000
#define MAX_COLS 1000

#define SPLITTER '^'
#define BEAM_START 'S'

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Put the input into a 2D array
    char input[MAX_ROWS][MAX_COLS];
    int rows = 0, cols = 0;
    int beamStartIdx;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        for (int idx = 0; idx < lineLen - 1; idx += 1) {
            input[rows][idx] = line[idx];

            if (line[idx] == BEAM_START) beamStartIdx = idx;
        }
        rows += 1;
        cols = lineLen - 1;
    }

    // For each level, we'll be keeping track of the current spots with beams.
    bool beams[cols];
    for (int idx = 0; idx < cols; idx += 1) beams[idx] = false;
    beams[beamStartIdx] = true;

    int numSplits = 0;
    for (int r = 0; r < rows; r += 1) {
        for (int c = 0; c < cols; c += 1) {
            // If a beam hit a splitter, count that and add beams to the left and right.
            // Since splitters can't be directly next to each other, we don't need to worry
            // about splitters on the same level interfering with each other.
            if (input[r][c] == SPLITTER && beams[c]) {
                numSplits += 1;
                beams[c] = false;

                beams[c - 1] = true;
                beams[c + 1] = true;
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

    char input[MAX_ROWS][MAX_COLS];
    int rows = 0, cols = 0;
    int beamStartIdx;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        for (int idx = 0; idx < lineLen - 1; idx += 1) {
            input[rows][idx] = line[idx];

            if (line[idx] == BEAM_START) beamStartIdx = idx;
        }
        rows += 1;
        cols = lineLen - 1;
    }

    long counts[cols];
    for (int idx = 0; idx < cols; idx += 1) counts[idx] = 0;
    counts[beamStartIdx] = 1;

    for (int r = 0; r < rows; r += 1) {
        for (int c = 0; c < cols; c += 1) {
            if (input[r][c] == SPLITTER && counts[c]) {
                counts[c - 1] += counts[c];
                counts[c + 1] += counts[c];
                counts[c] = 0;
            }
        }
    }

    // Get the total number of timelines (the total number of different beams making it to the final level).
    long total = 0;
    for (int c = 0; c < cols; c += 1) total += counts[c];

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
