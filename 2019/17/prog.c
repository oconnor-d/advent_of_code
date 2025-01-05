#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/intcode.c"

#define MAX_ROWS 100
#define MAX_COLS 100

void pushInputASCII(IntCodeProgram* program, char* asciiInput) {
    /*
    Pushes the string output as int ASCII codes to the program, ended with
    the newline code.
    */
    while (*asciiInput) {
        pushInput(program, (int)asciiInput[0]);
        asciiInput += 1;
    }

    pushInput(program, (int)'\n');
}

void problem1(char* inputFilePath) {
    /*
    The IntCode program outputs an ASCII-map, the problem is asking to find the number
    of intersections.

    Parse the output into a 2D array, and check for intersections that way. An intersection
    is a path-node in the map with path-nodes in all cardinal directions.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    intcodeRun(&program);

    char map[MAX_ROWS][MAX_COLS];

    int row = 0, col = 0, numCols = -1;
    for (int idx = 0; idx < program.output.numItems; idx += 1) {
        if ((char)program.output.data[idx] == '\n') {
            // Save the number of cols for the first time.
            if (numCols == -1) numCols = col + 1;

            row += 1;
            col = 0;

            continue;
        }

        map[row][col] = (char)program.output.data[idx];

        col += 1;
    }

    // Save the number of rows
    int numRows = row + 1;

    int totalAlignment = 0;
    for (row = 0; row < numRows; row += 1) {
        for (col = 0; col < numCols; col += 1) {
            // Can't have an intersection on the border of the map.
            if (row == 0 || row == numRows - 1 || col == 0 || col == numCols - 1) continue;

            if (
                map[row][col] == '#' &&
                // Check for a vertical path.
                map[row - 1][col] == '#' && map[row + 1][col] == '#' &&
                // Check for a horizontal path.
                map[row][col - 1] == '#' && map[row][col + 1] == '#') {
                totalAlignment += row * col;
            }
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", totalAlignment, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Program the cleaning robot to traverse the maze, and report it's final output.

    I did this one manually, by printing out the maze and building the commands all by hand.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    // Enable the bot.
    program.program[0] = 2;

    // Program
    pushInputASCII(&program, "A,B,B,A,C,A,C,A,C,B");

    // A
    pushInputASCII(&program, "R,6,R,6,R,8,L,10,L,4");
    // B
    pushInputASCII(&program, "R,6,L,10,R,8");
    // C
    pushInputASCII(&program, "L,4,L,12,R,6,L,10");
    // Continuous Feed
    pushInputASCII(&program, "n");

    intcodeRun(&program);

    // Print the feed, minus the final output which is the non-ASCII answer.
    for (int idx = 0; idx < program.output.numItems - 1; idx += 1) {
        printf("%c", (char)program.output.data[idx]);
    }
    printf("\n");

    long long dustCollected;
    popOutput(&program, &dustCollected);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", dustCollected, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
