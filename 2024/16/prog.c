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

int getDirectionScore(int direction1, int direction2) {
    /*
    Gets the score to go from one direction to another:
        1 to continue in the same direction
        + 1000 to turn from one direction to an adjacent one
    */
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
    /*
    Cheapest path from the start of a maze to the end, if moving in one direction costs 1 point
    and turning to face a new direction costs 1000 points.

    I wanted to use Dijkstra's for this, but I don't have a priority queue structure I can use,
    so I went with a DFS approach, storing the minimum cost of reaching a node in a separate
    map.

    If we encounter a node with a higher score, that path is a dead end, so we stop exploring it.

    Other than that, keep track of the minimum score when reaching the end, and you have your answer.
    */
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

// Min scores by (row, col, direction).
int minScores[INPUT_MAX_ROWS][INPUT_MAX_COLS][4];

bool markSeatsOnBestPath(int row, int col, char** map, int numRows, int numCols, int currentScore, int targetScore, int direction) {
    /*
    Recursively mark the seats on the best paths as visited.

    Generally, a node is on the best path if the score rest of the path (plus the score of the current node) is the cheapest path.
    This is figured out recursively, eventually the calls reach the end node, and from that point the call stack resolves
    the nodes in reverse order, from the end node to the start.
    */
    // If we've hit the END node with the correct score, we're on the best path (the base case).
    if (map[row][col] == END && currentScore == targetScore) return true;

    // If we've hit a WALL, or we've already exceeded the cheapest score, we're not on the best path.
    if (map[row][col] == WALL || currentScore > targetScore) return false;

    // If we've hit this node from this direction with a smaller score than the current on, we're not
    // on the best path.
    if (currentScore > minScores[row][col][direction]) return false;
    minScores[row][col][direction] = currentScore;

    // Recursively mark the seats from every possible direction, if one of the paths are a best path, then this node is on a best path.
    bool onBestPath = false;
    onBestPath = markSeatsOnBestPath(row - 1, col, map, numRows, numCols, currentScore + getDirectionScore(direction, NORTH), targetScore, NORTH);
    onBestPath = markSeatsOnBestPath(row, col + 1, map, numRows, numCols, currentScore + getDirectionScore(direction, EAST), targetScore, EAST) || onBestPath;
    onBestPath = markSeatsOnBestPath(row + 1, col, map, numRows, numCols, currentScore + getDirectionScore(direction, SOUTH), targetScore, SOUTH) || onBestPath;
    onBestPath = markSeatsOnBestPath(row, col - 1, map, numRows, numCols, currentScore + getDirectionScore(direction, WEST), targetScore, WEST) || onBestPath;

    // If we're on the best path, mark the node as visited.
    if (onBestPath) map[row][col] = BEST_PATH_NODE;

    return onBestPath;
}

void problem2(char* inputFilePath) {
    /*
    The count of all nodes on ANY (because there can be more than one) cheapest path from
    start to end.

    Again, Dijkstra's would have been perfect for this, but alas.

    We start by getting the cheapest path score by the same algorithm number one uses, and then we begin searching the maze again
    from the start, with the goal of marking all nodes in a path that leads to the end with the same score as the found
    cheapest score.

    When doing this second search, we again start keeping track of the minimum score at each node, but this time we need
    to take into account the direction we hit the node from, since the same node can have two different scores based on
    the direction it's entered from. The algorithm is very similar to the finding of the minimum score, but this time we
    recursively figure out if the REST of the path in any possible direction is the shortest path, and if there's a
    shortest path in at least one direction, the current node must also be in the shortest path.
    */
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
    int nodeScores[numRows][numCols];
    for (int r = 0; r < numRows; r += 1) {
        for (int c = 0; c < numCols; c += 1) {
            nodeScores[r][c] = INT_MAX;
        }
    }

    IntArray visitStack;
    initIntArray(&visitStack, numRows * numCols * 4);

    insertIntArray(&visitStack, startRow);
    insertIntArray(&visitStack, startCol);
    insertIntArray(&visitStack, 0);
    // The reindeer starts facing EAST.
    insertIntArray(&visitStack, EAST);

    // Get the cheapest score, as in part 1.
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

    // The score at the end node is the min score.
    nodeScores[endRow][endCol] = minScore;

    // Init the min scores.
    for (int r = 0; r < numRows; r += 1) {
        for (int c = 0; c < numCols; c += 1) {
            minScores[r][c][0] = INT_MAX;
            minScores[r][c][1] = INT_MAX;
            minScores[r][c][2] = INT_MAX;
            minScores[r][c][3] = INT_MAX;
        }
    }
    markSeatsOnBestPath(startRow, startCol, map, numRows, numCols, 0, minScore, EAST);
    // The end is always on the best path (obviously).
    map[endRow][endCol] = BEST_PATH_NODE;

    // Count the best seats.
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
