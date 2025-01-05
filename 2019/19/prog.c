#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/intcode.c"

#define MAX_ROWS 50
#define MAX_COLS 50

#define SHIP_LENGTH 100

void problem1(char* inputFilePath) {
    /*
    Simply provide all the coordinates in the given range, and count the number of coords in
    the beam range.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    long long countInBeamRange = 0;
    long long inBeamRange;
    for (int row = 0; row < MAX_ROWS; row += 1) {
        for (int col = 0; col < MAX_COLS; col += 1) {
            clearInput(&program);
            pushInput(&program, col);
            pushInput(&program, row);

            intcodeRun(&program);

            popOutput(&program, &inBeamRange);
            countInBeamRange += inBeamRange;
            if (inBeamRange)
                printf("#");
            else
                printf(".");
        }
        printf("\n");
    }

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", countInBeamRange, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Basically brute-forcing a bunch of different coordinates, but with more restrictions on the coords we check.

    Knowing that the beam is continuous, we can stop checking the current row when we've hit a non-beam space
    after the beam space, and the next row's beam must start at or after the last row's col.

    Whenever we hit a beam spot, check if the spot a ship's length down and right are also beam spots, if so,
    we've found the spot the ship can fit in.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    int startBeamCol = -1;
    long long inBeamRange;
    // Somewhat of an educated guess about a starting row, col the ship absolutely cannot be at.
    int row = SHIP_LENGTH * 5, col = SHIP_LENGTH;
    while (true) {
        clearInput(&program);
        pushInput(&program, col);
        pushInput(&program, row);

        intcodeRun(&program);

        popOutput(&program, &inBeamRange);

        if (!inBeamRange) {
            // Finished the beams for this row, continue on to the next
            if (startBeamCol != -1) {
                row += 1;
                col = startBeamCol;
                startBeamCol = -1;
            } else {
                // Haven't found the beam yet, keep going.
                col += 1;
            }
        } else {
            // Found the start of this row's beam.
            if (startBeamCol == -1) startBeamCol = col;

            // Check right
            clearInput(&program);
            pushInput(&program, col + SHIP_LENGTH - 1);
            pushInput(&program, row);

            intcodeRun(&program);

            popOutput(&program, &inBeamRange);
            if (!inBeamRange) {
                // Move on to check the next column.
                col += 1;
                continue;
            };

            // Check down
            clearInput(&program);
            pushInput(&program, col);
            pushInput(&program, row + SHIP_LENGTH - 1);

            intcodeRun(&program);

            popOutput(&program, &inBeamRange);

            // If both right and down are also in range, we've found a spot the ship can fit in.
            if (inBeamRange) break;

            // Otherwise, move on to the next column.
            col += 1;
        }
    }

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", col * 10000 + row, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
