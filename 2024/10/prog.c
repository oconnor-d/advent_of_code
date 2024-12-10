#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"

#define INPUT_MAX_ROWS 40
#define INPUT_MAX_COLS 40

#define TRAIL_HEAD 0
#define TRAIL_END 9

void problem1(char* inputFilePath) {
    /*
    The problem is asking for the number of unique TRAIL_ENDs reachable from each TRAIL_HEAD, called
    the TRAIL_HEAD's score. If a TRAIL_HEAD has several paths to the same TRAIL_HEAD, that still only
    counts as 1 score. A path is valid if it goes from 0 -> 1 -> ... -> 8 -> 9 with a constant slope
    of 1 between each node.

    This algorithm uses DFS to find the paths from each TRAIL_HEAD to each TRAIL_END, counting only
    the unique TRAIL_ENDs reachable from the specific TRAIL_HEAD as the score.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Read the input into a map.
    int map[INPUT_MAX_ROWS][INPUT_MAX_COLS];
    int row = 0, col = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        col = 0;
        for (; col < lineLen; col += 1) map[row][col] = line[col] - '0';
        row += 1;
    }

    int numRows = row, numCols = col;

    int totalScore = 0;

    // Since I don't have a true Stack data structure, treat this array as one:
    // Only add and take from the end.
    //
    // Stores the rows and cols of nodes like: [row1, col1, row2, col2, ...]
    IntArray visitStack;
    initIntArray(&visitStack, INPUT_MAX_ROWS * INPUT_MAX_COLS * 2);

    // Since I also don't have a Set, we'll keep track of already visited TRAIL_ENDs
    // for each TRAIL_HEAD in an array. It's not efficient but it's more than fast
    // enough for the input.
    //
    // Stores the rows and cols of TRAIL_ENDS like: [row1, col1, row2, col2, ...]
    bool alreadyReachedTrailEnd = false;
    IntArray scoredTrailEnds;
    initIntArray(&scoredTrailEnds, INPUT_MAX_ROWS * INPUT_MAX_COLS);

    // For each TRAIL_HEAD in the map, perform a DFS for the reachable TRAIL_ENDs.
    int score, node, nodeRow, nodeCol;
    for (int row = 0; row < numRows; row += 1) {
        for (int col = 0; col < numCols; col += 1) {
            // Ignore non-TRAIL_HEAD starting points.
            if (map[row][col] != TRAIL_HEAD) continue;

            // Start searching.
            score = 0;

            // Reset the scored trail ends for this new head.
            scoredTrailEnds.numItems = 0;

            // Reset the visited stack and add the current (row, col) as the starting point
            // for the DFS.
            visitStack.numItems = 0;
            insertIntArray(&visitStack, row);
            insertIntArray(&visitStack, col);

            // Perform a DFS, counting edges with a slope of 1 from the current node as valid
            // edges to travel, and stopping when you've hit a TRAIL_END.
            while (visitStack.numItems > 0) {
                // Pop the node's index off the top of the stack.
                nodeRow = visitStack.data[visitStack.numItems - 2];
                nodeCol = visitStack.data[visitStack.numItems - 1];
                visitStack.numItems -= 2;

                node = map[nodeRow][nodeCol];

                // If it's a TRAIL_END stop the search, and increase the score if it's the first time we've
                // reached it from the current TRAIL_HEAD.
                if (node == TRAIL_END) {
                    // Make sure we haven't already counted this TRAIL_END.
                    alreadyReachedTrailEnd = false;
                    for (int idx = 0; idx < scoredTrailEnds.numItems; idx += 2) {
                        if (scoredTrailEnds.data[idx] == nodeRow && scoredTrailEnds.data[idx + 1] == nodeCol) {
                            alreadyReachedTrailEnd = true;
                            continue;
                        }
                    }

                    // If this TRAIL_HEAD hasn't been counted towards the score yet, count it now and mark
                    // it as already scored.
                    if (!alreadyReachedTrailEnd) {
                        insertIntArray(&scoredTrailEnds, nodeRow);
                        insertIntArray(&scoredTrailEnds, nodeCol);
                        score += 1;
                    }

                    // We're done traveling this path, continue down the next one.
                    continue;
                }

                // Check that going North is a valid path to take.
                if (nodeRow - 1 >= 0 && map[nodeRow - 1][nodeCol] - node == 1) {
                    insertIntArray(&visitStack, nodeRow - 1);
                    insertIntArray(&visitStack, nodeCol);
                }

                // Check that going East is a valid path to take.
                if (nodeCol + 1 < numCols && map[nodeRow][nodeCol + 1] - node == 1) {
                    insertIntArray(&visitStack, nodeRow);
                    insertIntArray(&visitStack, nodeCol + 1);
                }

                // Check that going South is a valid path to take.
                if (nodeRow + 1 < numRows && map[nodeRow + 1][nodeCol] - node == 1) {
                    insertIntArray(&visitStack, nodeRow + 1);
                    insertIntArray(&visitStack, nodeCol);
                }

                // Check that going West is a valid path to take.
                if (nodeCol - 1 >= 0 && map[nodeRow][nodeCol - 1] - node == 1) {
                    insertIntArray(&visitStack, nodeRow);
                    insertIntArray(&visitStack, nodeCol - 1);
                }
            }

            // Increase the total score with the current TRAIL_HEADs score.
            totalScore += score;
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", totalScore, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Instead of counting the total score of the trailheads, it's asking for the trailheads'
    rating. Basically, instead of counting the number of unique TRAIL_ENDs reachable, it's asking
    for the total number of paths that go from TRAIL_HEAD to TRAIL_END.

    This is a simpler version of part 1 with the DFS algorithm used. This part's exactly the same,
    except we don't keep track of the TRAIL_ENDs we already visited. Every time we hit a TRAIL_END,
    we've found a valid path and increase the score by 1.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Read the input into a map.
    int map[INPUT_MAX_ROWS][INPUT_MAX_COLS];
    int row = 0, col = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        col = 0;
        for (; col < lineLen; col += 1) map[row][col] = line[col] - '0';
        row += 1;
    }

    int numRows = row, numCols = col;

    int totalRating = 0;

    // Since I don't have a true Stack data structure, treat this array as one:
    // Only add and take from the end.
    //
    // Stores the rows and cols of nodes like: [row1, col1, row2, col2, ...]
    IntArray visitStack;
    initIntArray(&visitStack, INPUT_MAX_ROWS * INPUT_MAX_COLS * 2);

    // For each TRAIL_HEAD, perform an exhaustive DFS to get all paths leading to a
    // TRAIL_END.
    int node, nodeRow, nodeCol;
    for (int row = 0; row < numRows; row += 1) {
        for (int col = 0; col < numCols; col += 1) {
            if (map[row][col] != TRAIL_HEAD) continue;

            visitStack.numItems = 0;
            insertIntArray(&visitStack, row);
            insertIntArray(&visitStack, col);

            while (visitStack.numItems > 0) {
                // Pop the node's index off the top of the stack.
                nodeRow = visitStack.data[visitStack.numItems - 2];
                nodeCol = visitStack.data[visitStack.numItems - 1];
                visitStack.numItems -= 2;

                node = map[nodeRow][nodeCol];

                // If it's the TRAIL_END, we've found a valid path so increase the rating.
                if (node == TRAIL_END) {
                    totalRating += 1;
                    continue;
                }

                // Check North
                if (nodeRow - 1 >= 0 && map[nodeRow - 1][nodeCol] - node == 1) {
                    insertIntArray(&visitStack, nodeRow - 1);
                    insertIntArray(&visitStack, nodeCol);
                }

                // Check East
                if (nodeCol + 1 < numCols && map[nodeRow][nodeCol + 1] - node == 1) {
                    insertIntArray(&visitStack, nodeRow);
                    insertIntArray(&visitStack, nodeCol + 1);
                }

                // Check South
                if (nodeRow + 1 < numRows && map[nodeRow + 1][nodeCol] - node == 1) {
                    insertIntArray(&visitStack, nodeRow + 1);
                    insertIntArray(&visitStack, nodeCol);
                }
                // Check West
                if (nodeCol - 1 >= 0 && map[nodeRow][nodeCol - 1] - node == 1) {
                    insertIntArray(&visitStack, nodeRow);
                    insertIntArray(&visitStack, nodeCol - 1);
                }
            }
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", totalRating, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
