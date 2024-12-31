#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/intcode.c"
#include "../../utils/map.c"

#define BLACK 0
#define WHITE 1

#define TURN_LEFT 0
#define TURN_RIGHT 1

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

void problem1(char* inputFilePath) {
    /*
    Given a hull-painting IntCode program, run it starting from a BLACK tile, providing it with the tile color
    a robot is currently on, and painting the tile with the program's output, and moving the robot using the
    program's output. Then, get the count of unique painted tiles.

    The painted tiles are kept track of in a map, and the program is run as per it's instructions until it
    halts. Once the program is halted, count the number of unique coords in the map.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    LLongMap visitedTileColors;
    initLLongMap(&visitedTileColors);

    char* key;
    long long tile;
    // The bot starts at (0, 0) facing up.
    int x = 0, y = 0, direction = UP;
    do {
        // Get the color of the current tile, defaulting to BLACK.
        key = malloc(25);
        sprintf(key, "%d,%d", x, y);

        if (!getLLongMap(&visitedTileColors, key, &tile)) {
            // All tiles start out as black
            tile = BLACK;
        }

        // Tell the program what color tile the bot's on.
        pushInput(&program, tile);

        // Run the program.
        intcodeRun(&program);

        // Paint the tile
        setLLongMap(&visitedTileColors, key, program.output.data[program.output.numItems - 2]);

        // Move the robot.
        if (program.output.data[program.output.numItems - 1] == TURN_LEFT) {
            if (direction == UP) {
                direction = LEFT;
                x -= 1;
            } else if (direction == LEFT) {
                direction = DOWN;
                y += 1;
            } else if (direction == DOWN) {
                direction = RIGHT;
                x += 1;
            } else if (direction == RIGHT) {
                direction = UP;
                y -= 1;
            }
        } else {
            if (direction == UP) {
                direction = RIGHT;
                x += 1;
            } else if (direction == RIGHT) {
                direction = DOWN;
                y += 1;
            } else if (direction == DOWN) {
                direction = LEFT;
                x -= 1;
            } else if (direction == LEFT) {
                direction = UP;
                y -= 1;
            }
        }
    } while (!program.halted);

    // Tally up the unique tiles that were painted.
    int total = 0;
    for (int idx = 0; idx < visitedTileColors.capacity; idx += 1) {
        if (visitedTileColors.entries[idx].key == NULL) continue;
        total += 1;
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", total, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Starting on a white tile this time, print the registration number the robot paints.

    Same algorithm as part 1, but starting from a white tile, and then the output grid gets
    printed with the registration code.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    LLongMap visitedTileColors;
    initLLongMap(&visitedTileColors);

    char* key;
    long long tile;
    // The bot starts at (0, 0) facing up.
    int x = 0, y = 0, direction = UP;

    // Keep track of maximum X and Y values for printing the grid later.
    // The input is nice enough to never bring you into negative coordinates if you start at (0, 0),
    // so it's safe to assume a 0, 0 start.
    int maxX = INT_MIN, maxY = INT_MIN;

    // Start the bot on a white tile.
    key = malloc(25);
    sprintf(key, "%d,%d", x, y);
    setLLongMap(&visitedTileColors, key, WHITE);

    do {
        // Get the tile color.
        key = malloc(25);
        sprintf(key, "%d,%d", x, y);

        if (!getLLongMap(&visitedTileColors, key, &tile)) {
            // All tiles start out as black
            tile = BLACK;
        }

        // Tell the program what color tile the bot's on.
        pushInput(&program, tile);

        // Run the program.
        intcodeRun(&program);

        // Paint the tile
        setLLongMap(&visitedTileColors, key, program.output.data[program.output.numItems - 2]);

        // Move the robot.
        if (program.output.data[program.output.numItems - 1] == TURN_LEFT) {
            if (direction == UP) {
                direction = LEFT;
                x -= 1;
            } else if (direction == LEFT) {
                direction = DOWN;
                y += 1;
            } else if (direction == DOWN) {
                direction = RIGHT;
                x += 1;
            } else if (direction == RIGHT) {
                direction = UP;
                y -= 1;
            }
        } else {
            if (direction == UP) {
                direction = RIGHT;
                x += 1;
            } else if (direction == RIGHT) {
                direction = DOWN;
                y += 1;
            } else if (direction == DOWN) {
                direction = LEFT;
                x -= 1;
            } else if (direction == LEFT) {
                direction = UP;
                y -= 1;
            }
        }

        // Keep track of the upper bounds of the grid the robot painted.
        if (x > maxX) maxX = x;
        if (y > maxY) maxY = y;
    } while (!program.halted);

    printf("Problem 02:\n");
    // Print the painted hull.
    for (y = 0; y < maxY + 1; y += 1) {
        for (x = 0; x < maxX + 1; x += 1) {
            key = malloc(25);
            sprintf(key, "%d,%d", x, y);
            if (!getLLongMap(&visitedTileColors, key, &tile)) {
                tile = BLACK;
            }

            if (tile == WHITE)
                printf("#");
            else
                printf(" ");
        }
        printf("\n");
    }
    clock_t end = clock();
    printf("[%.2fms]\n", (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
