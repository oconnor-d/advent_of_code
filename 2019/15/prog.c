#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../../utils/intcode.c"

#define HIT_WALL 0
#define MOVED 1
#define FOUND_OXYGEN_SYSTEM 2

#define MOVE_NORTH 1
#define MOVE_SOUTH 2
#define MOVE_WEST 3
#define MOVE_EAST 4

#define MAX_X 41
#define MAX_Y 41

#define DISPLAY_GAME false

void problem1(char* inputFilePath) {
    /*
    Given an unknown maze and an IntCode program to explore it, find the shortest path to a specific
    node in the maze.

    The first step is to explore the maze and build out a map of it. To do that, I gave the robot the
    "keep your left hand on the wall" algorithm - always prefer going left, when you hit a wall, turn
    right.

    That gives us a map we can then DFS to get back to the start point.

    This works because the maze is doesn't have any loops we get caught on.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    bool emptySpaces[MAX_Y][MAX_X];
    int nodeScores[MAX_Y][MAX_X];

    for (int row = 0; row < MAX_Y; row += 1) {
        for (int col = 0; col < MAX_X; col += 1) {
            emptySpaces[row][col] = false;
            nodeScores[row][col] = INT_MAX;
        }
    }

    // (21, 21) is an arbitrary starting point, which I got from figuring out the dimensions of the
    // maze manually.
    int x = 21, y = 21;
    emptySpaces[y][x] = true;

    // Left-hand walk through the maze until the O2 room is found.
    long long statusCode;
    int direction = MOVE_NORTH;
    pushInput(&program, direction);
    do {
        intcodeRun(&program);
        popOutput(&program, &statusCode);

        // If the droid walks into a wall it turns right. Otherwise it steps forward and turns left.
        if (statusCode == HIT_WALL) {
            if (direction == MOVE_NORTH)
                direction = MOVE_EAST;
            else if (direction == MOVE_EAST)
                direction = MOVE_SOUTH;
            else if (direction == MOVE_SOUTH)
                direction = MOVE_WEST;
            else if (direction == MOVE_WEST)
                direction = MOVE_NORTH;
        } else {
            if (direction == MOVE_NORTH) {
                y -= 1;
                direction = MOVE_WEST;
            } else if (direction == MOVE_EAST) {
                x += 1;
                direction = MOVE_NORTH;
            } else if (direction == MOVE_SOUTH) {
                y += 1;
                direction = MOVE_EAST;
            } else if (direction == MOVE_WEST) {
                x -= 1;
                direction = MOVE_SOUTH;
            }
            emptySpaces[y][x] = true;
        }

        pushInput(&program, direction);
    } while (statusCode != FOUND_OXYGEN_SYSTEM);

    int destY = y, destX = x;

    // Search for the shortest path.
    IntArray visitStack;
    initIntArray(&visitStack, MAX_Y * MAX_X * 2);

    // Starting from the start, DFS to the O2 room, keeping only the shortest path
    // there.
    insertIntArray(&visitStack, 21);
    insertIntArray(&visitStack, 21);
    insertIntArray(&visitStack, 0);

    int minScore = INT_MAX;
    int score;
    while (visitStack.numItems > 0) {
        score = popIntArray(&visitStack);
        x = popIntArray(&visitStack);
        y = popIntArray(&visitStack);

        if (x == destX && y == destY) {
            minScore = score;
            break;
        }

        if (score > nodeScores[y][x]) continue;
        nodeScores[y][x] = score;

        // NOTE: The input is bordered, so no need to bounds check.
        if (emptySpaces[y - 1][x]) {
            insertIntArray(&visitStack, y - 1);
            insertIntArray(&visitStack, x);
            insertIntArray(&visitStack, score + 1);
        }
        if (emptySpaces[y + 1][x]) {
            insertIntArray(&visitStack, y + 1);
            insertIntArray(&visitStack, x);
            insertIntArray(&visitStack, score + 1);
        }
        if (emptySpaces[y][x - 1]) {
            insertIntArray(&visitStack, y);
            insertIntArray(&visitStack, x - 1);
            insertIntArray(&visitStack, score + 1);
        }
        if (emptySpaces[y][x + 1]) {
            insertIntArray(&visitStack, y);
            insertIntArray(&visitStack, x + 1);
            insertIntArray(&visitStack, score + 1);
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", minScore, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    If the oxygen from the found room expands to all neighboring rooms once per minute, find the amount of
    time for the oxygen to spread throughout all empty spaces.

    Very similarly to part 1, use the left hand algorithm to map the maze, this time stopping when you've
    hit the starting point (from the starting direction), instead of when you hit the oxygen room. We need
    to make sure the whole maze is mapped, unlike part 1 where finding the O2 room was sufficient.

    Then, again like in part 1, do a DFS. This time do it until you've visited all the nodes, keeping track
    of the distance of each node to the O2 room, and keeping track of the greatest distance overall.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    char outputBuffer[MAX_Y][MAX_X];
    bool emptySpaces[MAX_Y][MAX_X];
    int nodeScores[MAX_Y][MAX_X];

    if (DISPLAY_GAME) {
        for (int row = 0; row < MAX_Y; row += 1) {
            for (int col = 0; col < MAX_X; col += 1) {
                outputBuffer[row][col] = '?';
            }
        }
    }

    for (int row = 0; row < MAX_Y; row += 1) {
        for (int col = 0; col < MAX_X; col += 1) {
            emptySpaces[row][col] = false;
            nodeScores[row][col] = INT_MAX;
        }
    }

    int destY, destX;
    int x = 21, y = 21;
    emptySpaces[y][x] = true;

    long long statusCode;
    int direction = MOVE_NORTH;
    pushInput(&program, direction);
    do {
        intcodeRun(&program);
        popOutput(&program, &statusCode);

        // If the droid walks into a wall it turns right.Otherwise it steps forward and turns left.
        if (statusCode == HIT_WALL) {
            if (direction == MOVE_NORTH)
                direction = MOVE_EAST;
            else if (direction == MOVE_EAST)
                direction = MOVE_SOUTH;
            else if (direction == MOVE_SOUTH)
                direction = MOVE_WEST;
            else if (direction == MOVE_WEST)
                direction = MOVE_NORTH;
        } else {
            if (direction == MOVE_NORTH) {
                y -= 1;
                direction = MOVE_WEST;
            } else if (direction == MOVE_EAST) {
                x += 1;
                direction = MOVE_NORTH;
            } else if (direction == MOVE_SOUTH) {
                y += 1;
                direction = MOVE_EAST;
            } else if (direction == MOVE_WEST) {
                x -= 1;
                direction = MOVE_SOUTH;
            }
            emptySpaces[y][x] = true;
        }

        // Store the O2 room to start the below DFS from.
        if (statusCode == FOUND_OXYGEN_SYSTEM) {
            destX = x;
            destY = y;
        }

        pushInput(&program, direction);

        if (DISPLAY_GAME) {
            if (statusCode == MOVED) {
                outputBuffer[y][x] = '.';
            } else if (statusCode == HIT_WALL) {
                // We've already changed direction from the direction the wall was in,
                // so these checks looks a little wonky
                if (direction == MOVE_EAST) outputBuffer[y - 1][x] = '#';
                if (direction == MOVE_WEST) outputBuffer[y + 1][x] = '#';
                if (direction == MOVE_SOUTH) outputBuffer[y][x + 1] = '#';
                if (direction == MOVE_NORTH) outputBuffer[y][x - 1] = '#';
            } else {
                outputBuffer[y][x] = 'X';
            }

            printf("%c[2J%c[;H", (char)27, (char)27);
            for (int row = 0; row < MAX_Y; row += 1) {
                for (int col = 0; col < MAX_X; col += 1) {
                    if (row == y && col == x && outputBuffer[row][col] != 'X')
                        printf("@");
                    else
                        printf("%c", outputBuffer[row][col]);
                }
                printf("\n");
            }
            usleep(6500);
        }

        // Once we've hit the start facing north again, we've mapped the entire maze.
        if (x == 21 && y == 21 && direction == MOVE_NORTH) break;
    } while (true);

    // DFS the entire maze, keeping the furthest distance.
    IntArray visitStack;
    initIntArray(&visitStack, MAX_Y * MAX_X * 2);

    insertIntArray(&visitStack, destY);
    insertIntArray(&visitStack, destX);
    insertIntArray(&visitStack, 0);

    int maxScore = INT_MIN;
    int score;
    while (visitStack.numItems > 0) {
        score = popIntArray(&visitStack);
        x = popIntArray(&visitStack);
        y = popIntArray(&visitStack);

        if (score > maxScore) maxScore = score;
        if (score > nodeScores[y][x]) continue;
        nodeScores[y][x] = score;

        // NOTE: The input is bordered, so no need to bounds check.
        if (emptySpaces[y - 1][x]) {
            insertIntArray(&visitStack, y - 1);
            insertIntArray(&visitStack, x);
            insertIntArray(&visitStack, score + 1);
        }
        if (emptySpaces[y + 1][x]) {
            insertIntArray(&visitStack, y + 1);
            insertIntArray(&visitStack, x);
            insertIntArray(&visitStack, score + 1);
        }
        if (emptySpaces[y][x - 1]) {
            insertIntArray(&visitStack, y);
            insertIntArray(&visitStack, x - 1);
            insertIntArray(&visitStack, score + 1);
        }
        if (emptySpaces[y][x + 1]) {
            insertIntArray(&visitStack, y);
            insertIntArray(&visitStack, x + 1);
            insertIntArray(&visitStack, score + 1);
        }

        outputBuffer[y][x] = '0';
        if (DISPLAY_GAME) {
            printf("%c[2J%c[;H", (char)27, (char)27);
            for (int row = 0; row < MAX_Y; row += 1) {
                for (int col = 0; col < MAX_X; col += 1) {
                    if (row == y && col == x && outputBuffer[row][col] != 'X')
                        printf("@");
                    else
                        printf("%c", outputBuffer[row][col]);
                }
                printf("\n");
            }
            usleep(6500);
        }
    }

    // Don't count the starting Oxygen room towards the total time taken.
    maxScore -= 1;

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", maxScore, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
