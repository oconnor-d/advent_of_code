#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"

#define INPUT_MAX_ROWS 150
#define INPUT_MAX_COLS 150
#define GUARD '^'
#define OBSTACLE '#'
#define EMPTY '.'
#define NORTH 'N'
#define EAST 'E'
#define SOUTH 'S'
#define WEST 'W'

bool traverseMap(char* map, int numRows, int numCols, int guardRow, int guardCol) {
    // Traverses the map, placing a direction char at each NEW spot the guard visits, optionally checking for lops.
    //
    // Assume the guard starts facing NORTH. Mark the guard's current location as visited in the direction they
    // are facing, and then lookahead at the next location.
    //
    // If it's an obstacle, turn 90 degrees, if it's an empty space, progress in that direction, and if
    // it's out of bounds or a loop, we're done.
    int direction = NORTH;
    while (true) {
        // If we hit an already VISITED spot going in the direction we FIRST hit at that spot, we're in a loop and have
        // found a valid new obstacle location.
        // If we changed direction on the last go, we haven't moved and would be on a VISITED square, so don't count that.
        if (map[guardRow * INPUT_MAX_COLS + guardCol] == direction) return true;

        // Make sure not to override the first direction a guard visits per spot.
        if (map[guardRow * INPUT_MAX_COLS + guardCol] == EMPTY || map[guardRow * INPUT_MAX_COLS + guardCol] == GUARD) map[guardRow * INPUT_MAX_COLS + guardCol] = direction;

        if (direction == NORTH) {
            // Out of bounds
            if (guardRow == 0) break;

            // If the space ahead is an obstacle, turn, otherwise move in the current direction.
            if (map[(guardRow - 1) * INPUT_MAX_COLS + guardCol] == OBSTACLE)
                direction = EAST;
            else
                guardRow -= 1;
        } else if (direction == EAST) {
            if (guardCol == numCols - 1) break;

            if (map[guardRow * INPUT_MAX_COLS + (guardCol + 1)] == OBSTACLE)
                direction = SOUTH;
            else
                guardCol += 1;
        } else if (direction == SOUTH) {
            if (guardRow == numRows - 1) break;

            if (map[(guardRow + 1) * INPUT_MAX_COLS + guardCol] == OBSTACLE)
                direction = WEST;
            else
                guardRow += 1;
        } else {
            if (guardCol == 0) break;

            if (map[guardRow * INPUT_MAX_COLS + (guardCol - 1)] == OBSTACLE)
                direction = NORTH;
            else
                guardCol -= 1;
        }
    }

    return false;
}

void problem1(char* inputFilePath) {
    /*
    The problem is asking to count the number of unique spots a security guard visits on their route,
    where they head straight until hitting an obstacle, turning 90 degrees clockwise when they do.

    The problem stops when they've walked out of bounds of the map.

    Read the input into a 2D array, and traverse it according to the above rules, nice and easy.
    */
    clock_t start = clock();

    // Assume each row has the same number of columns.
    FILE* inputFile = fopen(inputFilePath, "r");

    char input[(INPUT_MAX_ROWS * INPUT_MAX_COLS) + 1];
    size_t fileLen = fread(input, sizeof(char), INPUT_MAX_ROWS * INPUT_MAX_COLS, inputFile);

    fclose(inputFile);

    // Load the input into a 2D array for easier processing, and figure out the starting
    // position of the guard.
    int row = 0, col = 0, guardRow, guardCol;
    char* map = malloc((INPUT_MAX_ROWS * INPUT_MAX_COLS) * sizeof(char));
    for (int idx = 0; idx < fileLen; idx += 1) {
        if (input[idx] == GUARD) {
            guardRow = row;
            guardCol = col;
        }

        if (input[idx] == '\n') {
            row += 1;
            col = 0;
        } else {
            map[row * INPUT_MAX_COLS + col] = input[idx];
            col += 1;
        }
    }

    // Store the size of the 2D array.
    int numRows = row + 1, numCols = col;

    traverseMap(map, numRows, numCols, guardRow, guardCol);

    // Count the visited locations.
    int visitedLocations = 0;
    for (int row = 0; row < numRows; row += 1) {
        for (int col = 0; col < numCols; col += 1) {
            if (map[row * INPUT_MAX_COLS + col] == NORTH ||
                map[row * INPUT_MAX_COLS + col] == EAST ||
                map[row * INPUT_MAX_COLS + col] == SOUTH ||
                map[row * INPUT_MAX_COLS + col] == WEST)
                visitedLocations += 1;
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", visitedLocations, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    This problem is asking for the number of locations a single new obstacle can be placed to get the guard
    stuck in an infinite loop, following the same rules as in part 1.

    For each empty space (non-obstacle, non-guard), place a new obstacle and walk the map like in part 1, keeping
    track of the first direction the guard was facing when hitting a new space. If we revisit a space going the
    same direction as the first visit, we've hit a loop.

    This is improved by only placing new obstacles on the path the guard would travel in the original map. The
    code has a lot of duplication that can be improved, though.
    */
    clock_t start = clock();

    // Assume each row has the same number of columns.
    FILE* inputFile = fopen(inputFilePath, "r");

    char input[(INPUT_MAX_ROWS * INPUT_MAX_COLS) + 1];
    size_t fileLen = fread(input, sizeof(char), INPUT_MAX_ROWS * INPUT_MAX_COLS, inputFile);

    fclose(inputFile);

    // Load the input into a 2D array for easier processing, and figure out the starting
    // position of the guard.
    int row = 0, col = 0, guardStartRow, guardStartCol;
    char* originalMap = malloc((INPUT_MAX_ROWS * INPUT_MAX_COLS) * sizeof(char));
    for (int idx = 0; idx < fileLen; idx += 1) {
        if (input[idx] == GUARD) {
            guardStartRow = row;
            guardStartCol = col;
        }

        if (input[idx] == '\n') {
            row += 1;
            col = 0;
        } else {
            originalMap[row * INPUT_MAX_COLS + col] = input[idx];
            col += 1;
        }
    }

    // Store the size of the 2D array.
    int numRows = row + 1, numCols = col;

    // Traverse the original map to get the path the guard travels.
    traverseMap(originalMap, numRows, numCols, guardStartRow, guardStartCol);

    // Store the visited positions for new obstacle placement in the next part, and remove VISITED information
    // from the original map for easier checking in the next part.
    IntArray visitedPositions;
    initIntArray(&visitedPositions, numRows * numCols);
    for (int row = 0; row < numRows; row += 1) {
        for (int col = 0; col < numCols; col += 1) {
            if (originalMap[row * INPUT_MAX_COLS + col] == NORTH ||
                originalMap[row * INPUT_MAX_COLS + col] == EAST ||
                originalMap[row * INPUT_MAX_COLS + col] == SOUTH ||
                originalMap[row * INPUT_MAX_COLS + col] == WEST) {
                insertIntArray(&visitedPositions, row);
                insertIntArray(&visitedPositions, col);

                // Reset it back to an empty space.
                originalMap[row * INPUT_MAX_COLS + col] = EMPTY;
            }
        }
    }

    // Assume the guard starts facing NORTH. Mark the guard's current location as visited, and then
    // depending on the direction the guard is facing lookahead at the next location.
    //
    // If it's an obstacle, turn 90 degrees, if it's an empty space, progress in that direction, and if
    // it's out of bounds, we're done.
    char* map = malloc((INPUT_MAX_ROWS * INPUT_MAX_COLS) * sizeof(char));
    int newObstacles = 0;
    int guardRow = guardStartRow, guardCol = guardStartCol;
    for (int idx = 0; idx < visitedPositions.numItems; idx += 2) {
        row = visitedPositions.data[idx];
        col = visitedPositions.data[idx + 1];

        // Reset the map
        for (int r = 0; r < numRows; r += 1) {
            for (int c = 0; c < numCols; c += 1) {
                map[r * INPUT_MAX_COLS + c] = originalMap[r * INPUT_MAX_COLS + c];
            }
        }

        guardRow = guardStartRow;
        guardCol = guardStartCol;

        // We can't place an obstacle on the guard, move on to the next location.
        if (row == guardRow && col == guardCol) continue;
        // We can't place an obstacle on an original obstacle, move on to the next location.
        if (map[row * INPUT_MAX_COLS + col] == OBSTACLE) continue;

        // Place the new obstacle.
        map[row * INPUT_MAX_COLS + col] = OBSTACLE;

        if (traverseMap(map, numRows, numCols, guardRow, guardCol)) newObstacles += 1;
    }

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", newObstacles, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
