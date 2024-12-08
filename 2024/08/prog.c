#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INPUT_MAX_ROWS 50
#define INPUT_MAX_COLS 51

#define EMPTY_SPACE '.'
#define ANTINODE '#'

void problem1(char* inputFilePath) {
    /*
    The problem asks us to place "antinodes" in line with two nodes of the same frequency (represented by the same char),
    placed at the same distance away and in the same line as the two nodes. So:

    . . . .    # # . .
    . a . . -> . a . .
    . . a .    # a a #
    . . . .    . # . #

    This stores both the map as a 2D array and the coordinates of all nodes as an array of [(int)node, row, col, ...], and
    goes through the list of nodes, using the distance between and direction of two matching nodes to compute the location
    of the antinodes, which it places on the map.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char input[(INPUT_MAX_ROWS * INPUT_MAX_COLS) + 1];
    size_t fileLen = fread(input, sizeof(char), INPUT_MAX_ROWS * INPUT_MAX_COLS, inputFile);

    fclose(inputFile);

    // Store the map here, so we can easily place the antinodes later.
    char map[INPUT_MAX_ROWS][INPUT_MAX_COLS];

    // Store the coordinates of the nodes separately, since this is what we'll be processing. Cast the chars as
    // ints so we can store them alongside their coordinates, and we don't care what the actual char is anyway.
    // [(int)c1, row1, col2, (int)c2, row2, col2, ...]
    // Be aggressive with the sizing, assume every space has a node (which takes up three spaces in the array).
    int coordinates[((INPUT_MAX_ROWS * INPUT_MAX_COLS) + 1) * 3];

    // Load the input into a 2D array and an array of coordinates for each nodes for easier processing.
    int row = 0, col = 0, coordinateIdx = 0;
    for (int idx = 0; idx < fileLen; idx += 1) {
        if (input[idx] != EMPTY_SPACE && input[idx] != '\n') {
            coordinates[coordinateIdx] = (int)input[idx];
            coordinates[coordinateIdx + 1] = row;
            coordinates[coordinateIdx + 2] = col;

            coordinateIdx += 3;
        }

        if (input[idx] == '\n') {
            row += 1;
            col = 0;
        } else {
            map[row][col] = input[idx];
            col += 1;
        }
    }

    // Store grid dimensions for bounds checking.
    int numRows = row + 1, numCols = col;

    // Loop through one coordinate (c, row, col) group at a time. For each coordinate, go
    // through all following matching nodes to compute the antinodes.
    int node, checkRow, checkCol, dRow, dCol, antinodeRow, antinodeCol;
    for (int idx = 0; idx < coordinateIdx; idx += 3) {
        node = coordinates[idx];
        row = coordinates[idx + 1];
        col = coordinates[idx + 2];

        for (int checkIdx = idx + 3; checkIdx < coordinateIdx; checkIdx += 3) {
            // Skip non-matching nodes.
            if (coordinates[checkIdx] != node) continue;

            checkRow = coordinates[checkIdx + 1];
            checkCol = coordinates[checkIdx + 2];

            dRow = abs(checkRow - row);
            dCol = abs(checkCol - col);

            // First, check for the first antinode (directionally up).

            // The check-coordinate MUST be after the current one, so we'll always be subtracting the row distance
            // from the current (going directionally up).

            // Continue if we're out of bounds.
            if (row - dRow < 0) goto node2;
            antinodeRow = row - dRow;

            // If we the check-col is to the left of the current one, we have to place the first antinode rightward.
            if (checkCol < col) {
                // Out of bounds.
                if (col + dCol >= numCols) goto node2;

                antinodeCol = col + dCol;
                // Otherwise we need to place the first antinode leftward.
            } else {
                // Out of bounds.
                if (col - dCol < 0) goto node2;

                antinodeCol = col - dCol;
            }
            // If we got to this point, we have the first valid antinode, so place it.
            map[antinodeRow][antinodeCol] = ANTINODE;

        node2:

            // Now, check for the second antinode (directionally down).
            if (checkRow + dRow >= numRows) continue;
            antinodeRow = checkRow + dRow;

            // If we the check-col is to the left of the current one, we have to place the second antinode leftward.
            if (checkCol < col) {
                // Out of bounds.
                if (checkCol - dCol < 0) continue;

                antinodeCol = checkCol - dCol;
                // Otherwise we need to place the second antinode rightward.
            } else {
                // Out of bounds.
                if (checkCol + dCol >= numCols) continue;

                antinodeCol = checkCol + dCol;
            }

            // If we got to this point, we have the second valid antinode, so place it.
            map[antinodeRow][antinodeCol] = ANTINODE;
        }
    }

    // Count the placed antinodes.
    int numAntinodes = 0;
    for (int r = 0; r < numRows; r += 1) {
        for (int c = 0; c < numCols; c += 1) {
            if (map[r][c] == ANTINODE) numAntinodes += 1;
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", numAntinodes, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    This is the same as part 1 except the antinodes continue along the line two nodes create until
    out of bounds.

    This modifies part 1 to keep placing antinodes in a loop until it's out of bounds.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char input[(INPUT_MAX_ROWS * INPUT_MAX_COLS) + 1];
    size_t fileLen = fread(input, sizeof(char), INPUT_MAX_ROWS * INPUT_MAX_COLS, inputFile);

    fclose(inputFile);

    // Store the map here, so we can easily place the antinodes later.
    char map[INPUT_MAX_ROWS][INPUT_MAX_COLS];

    // Store the coordinates of the nodes separately, since this is what we'll be processing. Cast the chars as
    // ints so we can store them alongside their coordinates, and we don't care what the actual char is anyway.
    // [(int)c1, row1, col2, (int)c2, row2, col2, ...]
    // Be aggressive with the sizing, assume every space has a node (which takes up three spaces in the array).
    int coordinates[((INPUT_MAX_ROWS * INPUT_MAX_COLS) + 1) * 3];

    // Load the input into a 2D array and an array of coordinates for each nodes for easier processing.
    int row = 0, col = 0, coordinateIdx = 0;
    for (int idx = 0; idx < fileLen; idx += 1) {
        if (input[idx] != EMPTY_SPACE && input[idx] != '\n') {
            coordinates[coordinateIdx] = (int)input[idx];
            coordinates[coordinateIdx + 1] = row;
            coordinates[coordinateIdx + 2] = col;

            coordinateIdx += 3;
        }

        if (input[idx] == '\n') {
            row += 1;
            col = 0;
        } else {
            map[row][col] = input[idx];
            col += 1;
        }
    }

    // Store grid dimensions for bounds checking.
    int numRows = row + 1, numCols = col;

    // Loop through one coordinate (c, row, col) group at a time. For each coordinate, go
    // through all following matching nodes to compute the antinodes.
    int node, checkRow, checkCol, dRow, dCol, antinodeRow, antinodeCol, loopRow, loopCol;
    for (int idx = 0; idx < coordinateIdx; idx += 3) {
        node = coordinates[idx];
        row = coordinates[idx + 1];
        col = coordinates[idx + 2];

        for (int checkIdx = idx + 3; checkIdx < coordinateIdx; checkIdx += 3) {
            // Skip non-matching nodes.
            if (coordinates[checkIdx] != node) continue;

            checkRow = coordinates[checkIdx + 1];
            checkCol = coordinates[checkIdx + 2];

            dRow = abs(checkRow - row);
            dCol = abs(checkCol - col);

            // First, get the antinode above the first node's position (at (row, col)).
            loopRow = row;
            loopCol = col;

            // While we're in bounds, keep placing antinodes above the first node, using the last placed antinode
            // as the new starting location.
            while (true) {
                // The check-coordinate MUST be after the current one, so we'll always be subtracting the row distance
                // from the current node (going directionally up).

                if (loopRow - dRow < 0) break;
                antinodeRow = loopRow - dRow;
                loopRow -= dRow;

                // If we the check-col is to the left of the current one, we have to place the first antinode rightward.
                if (checkCol < loopCol) {
                    // Out of bounds.
                    if (loopCol + dCol >= numCols) break;

                    antinodeCol = loopCol + dCol;
                    loopCol += dCol;
                    // Otherwise we need to place the first antinode leftward.
                } else {
                    // Out of bounds.
                    if (loopCol - dCol < 0) break;

                    antinodeCol = loopCol - dCol;
                    loopCol -= dCol;
                }

                // If we got to this point, we have a valid antinode, so place it.
                map[antinodeRow][antinodeCol] = ANTINODE;
            }

            // Then, get the antinode below the second node's position (at (checkRow, checkCol)).
            loopRow = checkRow;
            loopCol = checkCol;

            // While we're in bounds, keep placing antinodes below the second node.
            while (true) {
                // Now, check for the second antinode (directionally down).
                if (loopRow + dRow >= numRows) break;
                antinodeRow = loopRow + dRow;
                loopRow += dRow;

                // If we the check-col is to the left of the current one, we have to place the second antinode leftward.
                if (loopCol < col) {
                    // Out of bounds.
                    if (loopCol - dCol < 0) break;

                    antinodeCol = loopCol - dCol;
                    loopCol -= dCol;
                    // Otherwise we need to place the second antinode rightward.
                } else {
                    // Out of bounds.
                    if (loopCol + dCol >= numCols) break;

                    antinodeCol = loopCol + dCol;
                    loopCol += dCol;
                }

                // If we got to this point, we have a valid antinode, so place it.
                map[antinodeRow][antinodeCol] = ANTINODE;
            }
        }
    }

    // Count the placed antinodes + the regular nodes which by definition are antinodes, just not
    // ones that we'd have computed above.
    int numAntinodes = 0;
    for (int r = 0; r < numRows; r += 1) {
        for (int c = 0; c < numCols; c += 1) {
            if (map[r][c] != EMPTY_SPACE) numAntinodes += 1;
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", numAntinodes, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
