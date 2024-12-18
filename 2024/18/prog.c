#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

#define MAX_BYTES 3450

#define WALL '#'
#define EMPTY_SPACE '.'

void problem1(char* inputFilePath) {
    /*
    Given a grid and bytes (walls) to place on the map, get the least amount of steps to get from
    the top left to the bottom right of the grid.

    Same exact idea as the other graph problem a few days ago, but this time I figured out how to
    do Dijkstra's correctly (even though BFS would work just as well since all the edge weights
    are the same).

    I modified the input to include the grid size and bytes to drop.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int parserEndIdx;
    // Parse the grid size and bytes to fall.
    int gridSize, bytesToFall;
    getline(&line, &lineCap, inputFile);
    gridSize = parseNumber(line, 0, &parserEndIdx);
    bytesToFall = parseNumber(line, parserEndIdx, &parserEndIdx);

    // Set up the grid, padding the input with walls along the borders so we don't have to bound
    // check in our search.
    //
    // Pad the size for adding in wall borders.
    char map[gridSize + 2][gridSize + 2];

    // Setup borders
    int bytesFallen = 0;
    for (int idx = 0; idx < gridSize + 2; idx += 1) {
        // Top and bottom.
        map[0][idx] = WALL;
        map[gridSize + 1][idx] = WALL;

        // Left and right.
        map[idx][0] = WALL;
        map[idx][gridSize + 1] = WALL;
    }

    // Setup empty space
    for (int r = 1; r < gridSize + 1; r += 1) {
        for (int c = 1; c < gridSize + 1; c += 1) {
            map[r][c] = EMPTY_SPACE;
        }
    }

    // Place fallen bytes.
    int row, col;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0 && bytesFallen < bytesToFall) {
        // Flipped because the input is x,y - or - col,row
        col = parseNumber(line, 0, &parserEndIdx);
        row = parseNumber(line, parserEndIdx, &parserEndIdx);

        map[row + 1][col + 1] = WALL;

        bytesFallen += 1;
    }

    fclose(inputFile);

    // Dijkstra's Algorithm
    int dist[gridSize + 2][gridSize + 2];
    bool shortestPath[gridSize + 2][gridSize + 2];

    // Init the min distance array to the max values to work down, and no coord is on
    // the shortest path (yet).
    for (int r = 0; r < gridSize + 2; r += 1) {
        for (int c = 0; c < gridSize + 2; c += 1) {
            dist[r][c] = INT_MAX;
            shortestPath[r][c] = false;
        }
    }

    // The dist to the maze start is always 0.
    dist[1][1] = 0;

    int minRow, minCol, minDistance;

    // For every row, col update the min scores.
    for (int count = 0; count < (gridSize + 2) * (gridSize + 2) - 1; count++) {
        // Get the next min distance not on the shortest path.
        minDistance = INT_MAX;
        for (int r = 0; r < gridSize + 2; r += 1) {
            for (int c = 0; c < gridSize + 2; c += 1) {
                if (shortestPath[r][c] == false && dist[r][c] <= minDistance) {
                    minDistance = dist[r][c];
                    minRow = r, minCol = c;
                }
            }
        }

        // Mark it as visited
        shortestPath[minRow][minCol] = true;

        // Update the distances of all 4 directions (if it's not a WALL).
        if (!shortestPath[minRow - 1][minCol] && map[minRow - 1][minCol] != WALL && dist[minRow][minCol] + 1 < dist[minRow - 1][minCol]) {
            dist[minRow - 1][minCol] = dist[minRow][minCol] + 1;
        }

        if (!shortestPath[minRow][minCol + 1] && map[minRow][minCol + 1] != WALL && dist[minRow][minCol] + 1 < dist[minRow][minCol + 1]) {
            dist[minRow][minCol + 1] = dist[minRow][minCol] + 1;
        }

        if (!shortestPath[minRow + 1][minCol] && map[minRow + 1][minCol] != WALL && dist[minRow][minCol] + 1 < dist[minRow + 1][minCol]) {
            dist[minRow + 1][minCol] = dist[minRow][minCol] + 1;
        }

        if (!shortestPath[minRow][minCol - 1] && map[minRow][minCol - 1] != WALL && dist[minRow][minCol] + 1 < dist[minRow][minCol - 1]) {
            dist[minRow][minCol - 1] = dist[minRow][minCol] + 1;
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", dist[gridSize][gridSize], (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Find the first byte in the list of bytes that makes the max impossible to complete.

    Same algorithm as part 1, this time with a binary search for the number of bytes to drop
    to find the threshold where the maze becomes un-completable.

    Run Dijkstra's to figure out if the maze is completable, if it is increase the number of
    bytes, if it's not decrease them. Easy as that.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int parserEndIdx;
    // Parse the grid size (we ignore the bytes to fall for this part).
    int gridSize;
    getline(&line, &lineCap, inputFile);
    gridSize = parseNumber(line, 0, &parserEndIdx);

    // Store the input bytes in a 2D array.
    int bytes[MAX_BYTES][2];
    int numBytes = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        bytes[numBytes][1] = parseNumber(line, 0, &parserEndIdx);
        bytes[numBytes][0] = parseNumber(line, parserEndIdx, &parserEndIdx);
        numBytes += 1;
    }

    fclose(inputFile);

    // Pad for adding in wall borders.
    char map[gridSize + 2][gridSize + 2];

    // Setup borders
    for (int idx = 0; idx < gridSize + 2; idx += 1) {
        // Top and bottom.
        map[0][idx] = WALL;
        map[gridSize + 1][idx] = WALL;

        // Left and right.
        map[idx][0] = WALL;
        map[idx][gridSize + 1] = WALL;
    }

    // Setup empty space
    for (int r = 1; r < gridSize + 1; r += 1) {
        for (int c = 1; c < gridSize + 1; c += 1) {
            map[r][c] = EMPTY_SPACE;
        }
    }

    // Performs a binary search on the number of bytes to drop, if the path is completable at the current number
    // of drops, increase the number of drops, otherwise decrease it. Once the minBytes and maxBytes (the bounds
    // of the search) are off by one, we've found the point where the graph moves from completable to un-completable.
    bool pathCompletable = false;
    int minBytes = 0, maxBytes = numBytes, bytesToDrop;
    while (true) {
        // We've found the number of bytes.
        if (abs(minBytes - maxBytes) == 1) break;

        // Get the half point between the bounds.
        bytesToDrop = minBytes + ((maxBytes - minBytes) / 2);

        // Reset the empty space.
        for (int r = 1; r < gridSize + 1; r += 1) {
            for (int c = 1; c < gridSize + 1; c += 1) {
                map[r][c] = EMPTY_SPACE;
            }
        }

        // Replace the bytes.
        for (int idx = 0; idx < bytesToDrop; idx += 1) {
            map[bytes[idx][0] + 1][bytes[idx][1] + 1] = WALL;
        }

        // Perform Dijkstra's
        int dist[gridSize + 2][gridSize + 2];
        bool shortestPath[gridSize + 2][gridSize + 2];

        for (int r = 0; r < gridSize + 2; r += 1) {
            for (int c = 0; c < gridSize + 2; c += 1) {
                dist[r][c] = INT_MAX;
                shortestPath[r][c] = false;
            }
        }

        // The dist to the maze start is always 0.
        dist[1][1] = 0;

        int minRow, minCol, minDistance;

        for (int count = 0; count < (gridSize + 2) * (gridSize + 2) - 1; count += 1) {
            minDistance = INT_MAX;
            for (int r = 0; r < gridSize + 2; r += 1) {
                for (int c = 0; c < gridSize + 2; c += 1) {
                    if (shortestPath[r][c] == false && dist[r][c] <= minDistance) {
                        minDistance = dist[r][c];
                        minRow = r, minCol = c;
                    }
                }
            }

            shortestPath[minRow][minCol] = true;

            if (!shortestPath[minRow - 1][minCol] && map[minRow - 1][minCol] != WALL && dist[minRow][minCol] + 1 < dist[minRow - 1][minCol]) {
                dist[minRow - 1][minCol] = dist[minRow][minCol] + 1;
            }

            if (!shortestPath[minRow][minCol + 1] && map[minRow][minCol + 1] != WALL && dist[minRow][minCol] + 1 < dist[minRow][minCol + 1]) {
                dist[minRow][minCol + 1] = dist[minRow][minCol] + 1;
            }

            if (!shortestPath[minRow + 1][minCol] && map[minRow + 1][minCol] != WALL && dist[minRow][minCol] + 1 < dist[minRow + 1][minCol]) {
                dist[minRow + 1][minCol] = dist[minRow][minCol] + 1;
            }

            if (!shortestPath[minRow][minCol - 1] && map[minRow][minCol - 1] != WALL && dist[minRow][minCol] + 1 < dist[minRow][minCol - 1]) {
                dist[minRow][minCol - 1] = dist[minRow][minCol] + 1;
            }
        }

        // If the path is completable, increase the lower bounds to the current number of bytes, otherwise decrease the
        // upper bounds.
        pathCompletable = dist[gridSize][gridSize] < INT_MAX && dist[gridSize][gridSize] >= 0;

        if (!pathCompletable) {
            // Drop more bytes
            maxBytes = bytesToDrop;
        } else {
            // Drop less bytes
            minBytes = bytesToDrop;
        }
    }

    // minBytes is the index in the bytes array for the first byte to make the path un-completable.
    int byteRow = bytes[minBytes][0];
    int byteCol = bytes[minBytes][1];

    clock_t end = clock();
    printf("Problem 02: %d,%d [%.2fms]\n", byteCol, byteRow, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
