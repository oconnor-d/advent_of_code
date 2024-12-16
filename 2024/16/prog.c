#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"

#define INPUT_MAX_ROWS 141
#define INPUT_MAX_COLS 141

#define START 'S'
#define END 'E'
#define WALL '#'
#define EMPTY_SPACE '.'
#define BEST_PATH_NODE 'O'

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define MOVE_COST 1
#define TURN_COST 1000

int minDistance[INPUT_MAX_ROWS][INPUT_MAX_COLS][4];

int getDirectionScore(int direction1, int direction2) {
    if (direction1 == direction2) return 1;

    if (direction1 == NORTH && direction2 == EAST) return 1001;
    if (direction1 == NORTH && direction2 == WEST) return 1001;
    if (direction1 == NORTH && direction2 == SOUTH) return 2001;

    if (direction1 == EAST && direction2 == NORTH) return 1001;
    if (direction1 == EAST && direction2 == SOUTH) return 1001;
    if (direction1 == EAST && direction2 == WEST) return 2001;

    if (direction1 == SOUTH && direction2 == EAST) return 1001;
    if (direction1 == SOUTH && direction2 == WEST) return 1001;
    if (direction1 == SOUTH && direction2 == NORTH) return 2001;

    if (direction1 == WEST && direction2 == NORTH) return 1001;
    if (direction1 == WEST && direction2 == SOUTH) return 1001;
    if (direction1 == WEST && direction2 == EAST) return 2001;

    // Satisfy the compiler, if we've hit this direction1 == direction2.
    return 1;
}

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Read the input into a map.
    char map[INPUT_MAX_ROWS][INPUT_MAX_COLS];
    int row = 0, col = 0, startRow, startCol;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        col = 0;
        for (; col < lineLen; col += 1) {
            map[row][col] = line[col];

            if (map[row][col] == START) startRow = row, startCol = col;
        }
        row += 1;
    }

    fclose(inputFile);

    int numRows = row, numCols = col;

    // Keep track of the lowest scores to reach each node.
    int nodeScores[numRows][numCols];
    for (int r = 0; r < numRows; r += 1) {
        for (int c = 0; c < numCols; c += 1) {
            nodeScores[r][c] = INT_MAX;
        }
    }

    // Dijkstra's Algorithm is required, but we can actually start with depth-first,
    // keeping track of minimum scores, let's start there.

    IntArray visitStack;
    initIntArray(&visitStack, numRows * numCols * 4);

    insertIntArray(&visitStack, startRow);
    insertIntArray(&visitStack, startCol);
    insertIntArray(&visitStack, 0);
    // The reindeer starts facing EAST.
    insertIntArray(&visitStack, EAST);

    int minScore = INT_MAX;
    int score, direction;
    while (visitStack.numItems > 0) {
        direction = popIntArray(&visitStack);
        score = popIntArray(&visitStack);
        col = popIntArray(&visitStack);
        row = popIntArray(&visitStack);

        // If we've reached the end
        if (map[row][col] == END) {
            minScore = score < minScore ? score : minScore;
            continue;
        }

        if (score > nodeScores[row][col]) continue;
        nodeScores[row][col] = score;

        // NOTE: The input is bordered, so no need to bounds check.

        // N
        if (map[row - 1][col] == EMPTY_SPACE || map[row - 1][col] == END) {
            insertIntArray(&visitStack, row - 1);
            insertIntArray(&visitStack, col);
            insertIntArray(&visitStack, score + getDirectionScore(direction, NORTH));
            insertIntArray(&visitStack, NORTH);
        }

        // E
        if (map[row][col + 1] == EMPTY_SPACE || map[row][col + 1] == END) {
            insertIntArray(&visitStack, row);
            insertIntArray(&visitStack, col + 1);
            insertIntArray(&visitStack, score + getDirectionScore(direction, EAST));
            insertIntArray(&visitStack, EAST);
        }

        // S
        if (map[row + 1][col] == EMPTY_SPACE || map[row + 1][col] == END) {
            insertIntArray(&visitStack, row + 1);
            insertIntArray(&visitStack, col);
            insertIntArray(&visitStack, score + getDirectionScore(direction, SOUTH));
            insertIntArray(&visitStack, SOUTH);
        }

        // W
        if (map[row][col - 1] == EMPTY_SPACE || map[row][col - 1] == END) {
            insertIntArray(&visitStack, row);
            insertIntArray(&visitStack, col - 1);
            insertIntArray(&visitStack, score + getDirectionScore(direction, WEST));
            insertIntArray(&visitStack, WEST);
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", minScore, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

bool markSeatsOnBestPath(int row, int col, int** nodeScores, char** map, int numRows, int numCols, int currentScore, int targetScore, int direction) {
    if (map[row][col] == END && currentScore == targetScore) return true;

    if (map[row][col] == WALL || currentScore > targetScore) return false;

    if (currentScore > minDistance[row][col][direction]) return false;
    minDistance[row][col][direction] = currentScore;

    bool onBestPath = false;
    onBestPath = markSeatsOnBestPath(row - 1, col, nodeScores, map, numRows, numCols, currentScore + getDirectionScore(direction, NORTH), targetScore, NORTH);
    onBestPath = markSeatsOnBestPath(row, col + 1, nodeScores, map, numRows, numCols, currentScore + getDirectionScore(direction, EAST), targetScore, EAST) || onBestPath;
    onBestPath = markSeatsOnBestPath(row + 1, col, nodeScores, map, numRows, numCols, currentScore + getDirectionScore(direction, SOUTH), targetScore, SOUTH) || onBestPath;
    onBestPath = markSeatsOnBestPath(row, col - 1, nodeScores, map, numRows, numCols, currentScore + getDirectionScore(direction, WEST), targetScore, WEST) || onBestPath;

    if (onBestPath) map[row][col] = BEST_PATH_NODE;

    return onBestPath;
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Read the input into a map.
    StringArray allocMap;
    initStringArray(&allocMap, INPUT_MAX_ROWS, INPUT_MAX_COLS);

    char** map = allocMap.data;
    int row = 0, col = 0, startRow, startCol, endRow, endCol;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        col = 0;
        for (; col < lineLen; col += 1) {
            map[row][col] = line[col];

            if (map[row][col] == START) startRow = row, startCol = col;
            if (map[row][col] == END) endRow = row, endCol = col;
        }
        row += 1;
    }

    fclose(inputFile);

    int numRows = row, numCols = col;


    // Keep track of the lowest scores to reach each node.
    int** nodeScores = malloc(numRows * sizeof(int*));
    for (int idx = 0; idx < numCols; idx += 1) nodeScores[idx] = malloc(numCols * sizeof(int));

    for (int r = 0; r < numRows; r += 1) {
        for (int c = 0; c < numCols; c += 1) {
            nodeScores[r][c] = INT_MAX;
        }
    }

    // Dijkstra's Algorithm is required, but we can actually start with depth-first,
    // keeping track of minimum scores, let's start there.

    IntArray visitStack;
    initIntArray(&visitStack, numRows * numCols * 4);

    insertIntArray(&visitStack, startRow);
    insertIntArray(&visitStack, startCol);
    insertIntArray(&visitStack, 0);
    // The reindeer starts facing EAST.
    insertIntArray(&visitStack, EAST);

    int minScore = INT_MAX;
    int score, direction;
    while (visitStack.numItems > 0) {
        direction = popIntArray(&visitStack);
        score = popIntArray(&visitStack);
        col = popIntArray(&visitStack);
        row = popIntArray(&visitStack);

        // If we've reached the end
        if (map[row][col] == END) {
            minScore = score < minScore ? score : minScore;
            continue;
        }

        if (score > nodeScores[row][col]) continue;
        nodeScores[row][col] = score;

        // NOTE: The input is bordered, so no need to bounds check.

        // N
        if (map[row - 1][col] == EMPTY_SPACE || map[row - 1][col] == END) {
            insertIntArray(&visitStack, row - 1);
            insertIntArray(&visitStack, col);
            insertIntArray(&visitStack, score + getDirectionScore(direction, NORTH));
            insertIntArray(&visitStack, NORTH);
        }

        // E
        if (map[row][col + 1] == EMPTY_SPACE || map[row][col + 1] == END) {
            insertIntArray(&visitStack, row);
            insertIntArray(&visitStack, col + 1);
            insertIntArray(&visitStack, score + getDirectionScore(direction, EAST));
            insertIntArray(&visitStack, EAST);
        }

        // S
        if (map[row + 1][col] == EMPTY_SPACE || map[row + 1][col] == END) {
            insertIntArray(&visitStack, row + 1);
            insertIntArray(&visitStack, col);
            insertIntArray(&visitStack, score + getDirectionScore(direction, SOUTH));
            insertIntArray(&visitStack, SOUTH);
        }

        // W
        if (map[row][col - 1] == EMPTY_SPACE || map[row][col - 1] == END) {
            insertIntArray(&visitStack, row);
            insertIntArray(&visitStack, col - 1);
            insertIntArray(&visitStack, score + getDirectionScore(direction, WEST));
            insertIntArray(&visitStack, WEST);
        }
    }

    nodeScores[endRow][endCol] = minScore;

    for (int r = 0; r < numRows; r += 1) {
        for (int c = 0; c < numCols; c += 1) {
            minDistance[r][c][0] = INT_MAX;
            minDistance[r][c][1] = INT_MAX;
            minDistance[r][c][2] = INT_MAX;
            minDistance[r][c][3] = INT_MAX;
        }
    }
    markSeatsOnBestPath(startRow, startCol, nodeScores, map, numRows, numCols, 0, minScore, EAST);
    // The end is always on the best path (obviously).
    map[endRow][endCol] = BEST_PATH_NODE;

    int seatsOnBestPath = 0;
    for (int r = 0; r < numRows; r += 1) {
        for (int c = 0; c < numCols; c += 1) {
            if (map[r][c] == BEST_PATH_NODE) seatsOnBestPath += 1;
        }
    }

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", seatsOnBestPath, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
