#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/map.c"

#define INPUT_MAX_ROWS 141
#define INPUT_MAX_COLS 141

#define START 'S'
#define END 'E'
#define WALL '#'

void problem1(char* inputFilePath) {
    /*
    Given a one-path maze, you can cheat through a wall for 2 picoseconds (i.e. two steps). Figure out the
    number of cheats that can save you more than a certain number of seconds.

    Traverse the maze once to get the "score" at each node from start to end, and then go through the entire
    grid again, and try removing a wall, getting the cheapest and most expensive non-wall paths reachable from
    it. If there's a reachable low and high score (i.e. the earliest and latest node in the maze's path),
    calculate a new score as if the wall doesn't exist.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    char map[INPUT_MAX_ROWS][INPUT_MAX_COLS];
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

    // Keep track of the steps to reach each node. INT_MAX = Unreachable.
    int nodeScores[numRows][numCols];
    for (int r = 0; r < numRows; r += 1) {
        for (int c = 0; c < numCols; c += 1) {
            nodeScores[r][c] = INT_MAX;
        }
    }

    row = startRow, col = startCol;

    // Perform the full traversal to get the score at each node.
    int stepsTaken = 0;
    while (row != endRow || col != endCol) {
        nodeScores[row][col] = stepsTaken;

        // NOTE: the input is bordered, so no need to bounds check.

        // Get the next space to go to.
        if (map[row - 1][col] != WALL && nodeScores[row - 1][col] == INT_MAX) {
            row -= 1;
        } else if (map[row][col + 1] != WALL && nodeScores[row][col + 1] == INT_MAX) {
            col += 1;
        } else if (map[row + 1][col] != WALL && nodeScores[row + 1][col] == INT_MAX) {
            row += 1;
        } else if (nodeScores[row][col - 1] == INT_MAX) {
            col -= 1;
        }

        stepsTaken += 1;
    }
    nodeScores[endRow][endCol] = stepsTaken;

    int cheatsSavingAtLeastOneHundredSeconds = 0;
    int baseScore = stepsTaken;
    int cheatScore, cheatStartScore, cheatEndScore;
    // Traverse the graph again, this time cheating. Don't traverse the outer boundaries.
    for (row = 1; row < numRows - 1; row += 1) {
        for (col = 1; col < numCols - 1; col += 1) {
            // Cheats start by pretending a wall doesn't exist.
            if (nodeScores[row][col] != INT_MAX) continue;

            // Try cheats, going forward!
            cheatStartScore = INT_MAX, cheatEndScore = -1;
            if (nodeScores[row - 1][col] != INT_MAX) {
                cheatStartScore = nodeScores[row - 1][col] < cheatStartScore ? nodeScores[row - 1][col] : cheatStartScore;
                cheatEndScore = nodeScores[row - 1][col] > cheatEndScore ? nodeScores[row - 1][col] : cheatEndScore;
            }
            if (nodeScores[row][col + 1] != INT_MAX) {
                cheatStartScore = nodeScores[row][col + 1] < cheatStartScore ? nodeScores[row][col + 1] : cheatStartScore;
                cheatEndScore = nodeScores[row][col + 1] > cheatEndScore ? nodeScores[row][col + 1] : cheatEndScore;
            }
            if (nodeScores[row + 1][col] != INT_MAX) {
                cheatStartScore = nodeScores[row + 1][col] < cheatStartScore ? nodeScores[row + 1][col] : cheatStartScore;
                cheatEndScore = nodeScores[row + 1][col] > cheatEndScore ? nodeScores[row + 1][col] : cheatEndScore;
            }
            if (nodeScores[row][col - 1] != INT_MAX) {
                cheatStartScore = nodeScores[row][col - 1] < cheatStartScore ? nodeScores[row][col - 1] : cheatStartScore;
                cheatEndScore = nodeScores[row][col - 1] > cheatEndScore ? nodeScores[row][col - 1] : cheatEndScore;
            }

            if (cheatStartScore != INT_MAX && cheatEndScore != INT_MAX) {
                // The number of steps to the cheat score, the steps taken during the cheat, and the steps taken
                // from the cheat end to the maze end.
                cheatScore = cheatStartScore + 2 + (baseScore - cheatEndScore);

                if (baseScore - cheatScore >= 100) cheatsSavingAtLeastOneHundredSeconds += 1;
            }
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", cheatsSavingAtLeastOneHundredSeconds, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Same as part 1, but this time a cheat can last 20 seconds.

    I got a working solution at first following a similar idea as part 1, but this time instead of removing a single wall, performing
    a DFS from each empty space to all other empty spaces (given they're within 20 steps). This worked but was too slow.

    Instead, store the coords of the nodes in the path, and do a much simpler and much quicker manhattan distance calculation to
    see if you can cheat from the starting node to the ending node within 20 steps.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    char map[INPUT_MAX_ROWS][INPUT_MAX_COLS];
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

    // Keep track of the steps to reach each node. INT_MAX = Unreachable.
    int nodeScores[numRows][numCols];
    for (int r = 0; r < numRows; r += 1) {
        for (int c = 0; c < numCols; c += 1) {
            nodeScores[r][c] = INT_MAX;
        }
    }

    row = startRow, col = startCol;

    // Perform the full traversal to get the score at each node, storing the coords on the path in
    // an array
    IntArray path;
    initIntArray(&path, INPUT_MAX_ROWS * INPUT_MAX_COLS);
    int stepsTaken = 0;
    while (row != endRow || col != endCol) {
        nodeScores[row][col] = stepsTaken;
        insertIntArray(&path, row);
        insertIntArray(&path, col);

        // NOTE: the input is bordered, so no need to bounds check.

        // Get the next space to go to.
        if (map[row - 1][col] != WALL && nodeScores[row - 1][col] == INT_MAX) {
            row -= 1;
        } else if (map[row][col + 1] != WALL && nodeScores[row][col + 1] == INT_MAX) {
            col += 1;
        } else if (map[row + 1][col] != WALL && nodeScores[row + 1][col] == INT_MAX) {
            row += 1;
        } else if (nodeScores[row][col - 1] == INT_MAX) {
            col -= 1;
        }

        stepsTaken += 1;
    }
    nodeScores[endRow][endCol] = stepsTaken;
    insertIntArray(&path, endRow);
    insertIntArray(&path, endCol);

    int cheatsSavingAtLeastOneHundredSeconds = 0;

    int baseScore = stepsTaken;
    int distance, cheatScore;
    for (int startIdx = 0; startIdx < path.numItems; startIdx += 2) {
        startRow = path.data[startIdx];
        startCol = path.data[startIdx + 1];

        for (int endIdx = startIdx + 2; endIdx < path.numItems; endIdx += 2) {
            endRow = path.data[endIdx];
            endCol = path.data[endIdx + 1];

            // Manhattan Distance.
            distance = abs(startRow - endRow) + abs(startCol - endCol);
            if (distance <= 20) {
                cheatScore = nodeScores[startRow][startCol] + distance + (baseScore - nodeScores[endRow][endCol]);
                if (baseScore - cheatScore >= 100) cheatsSavingAtLeastOneHundredSeconds += 1;
            }
        }
    }

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", cheatsSavingAtLeastOneHundredSeconds, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
