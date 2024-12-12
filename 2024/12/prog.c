#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"

#define INPUT_MAX_ROWS 140
#define INPUT_MAX_COLS 140

void problem1(char* inputFilePath) {
    /*
    The problem is asking for the sum the area and perimeter of each region (consecutive sequence of neighboring
    plots) in a garden map.

    This is done using a DFS-based "fill" or "flood" algorithm, where for each unvisited plot in the map
    it counts it's area and keeps searching until it hits a map edge or a different plot that's not part of the region,
    which is a perimeter of the region.

    So, we're doing the "fill" algorithm without actual filling the plots we traverse, we just take advantage of
    not revisiting plots, which would throw off the area and perimeter counts.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Read the input into a map.
    char map[INPUT_MAX_ROWS][INPUT_MAX_COLS];
    int row = 0, col = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        col = 0;
        for (; col < lineLen; col += 1) map[row][col] = line[col];
        row += 1;
    }

    int numRows = row, numCols = col;

    // Since I don't have a true Stack data structure, treat this array as one:
    // Only add and take from the end.
    //
    // Stores the rows and cols of nodes like: [row1, col1, row2, col2, ...]
    IntArray visitStack;
    initIntArray(&visitStack, INPUT_MAX_ROWS * INPUT_MAX_COLS * 2);

    // Keep track of already visited indices. 0 = Unvisited, 1 = Visited
    int visited[numRows][numCols];
    memset(visited, 0, numRows * numCols * sizeof(int));

    // For each unvisited plot, flood it to get the area and perimeter of the region.
    char plot;
    int totalFenceCost = 0, area, perimeter, nodeRow, nodeCol;
    for (int row = 0; row < numRows; row += 1) {
        for (int col = 0; col < numCols; col += 1) {
            // Already processed this region, so skip it.
            if (visited[row][col] == 1) continue;

            plot = map[row][col];
            area = 0;
            perimeter = 0;

            // Reset the visited stack and add the current (row, col) as the starting point
            // for the DFS.
            visitStack.numItems = 0;
            insertIntArray(&visitStack, row);
            insertIntArray(&visitStack, col);

            // Perform a DFS-based fill algorithm, searching until a map edge or different region is encountered,
            // only exploring a path if we haven't visited it before to avoid double counting area and/or perimeter.
            while (visitStack.numItems > 0) {
                // Pop the node's index off the top of the stack.
                nodeRow = visitStack.data[visitStack.numItems - 2];
                nodeCol = visitStack.data[visitStack.numItems - 1];
                visitStack.numItems -= 2;

                // Skip if we've already processed this plot.
                if (visited[nodeRow][nodeCol] == 1) continue;

                area += 1;
                visited[nodeRow][nodeCol] = 1;

                // Check that going North is a part of this region, otherwise we're at a perimeter.
                if (nodeRow - 1 >= 0 && map[nodeRow - 1][nodeCol] == plot) {
                    insertIntArray(&visitStack, nodeRow - 1);
                    insertIntArray(&visitStack, nodeCol);
                } else {
                    perimeter += 1;
                }

                // Check that going East is a part of this region, otherwise we're at a perimeter.
                if (nodeCol + 1 < numCols && map[nodeRow][nodeCol + 1] == plot) {
                    insertIntArray(&visitStack, nodeRow);
                    insertIntArray(&visitStack, nodeCol + 1);
                } else {
                    perimeter += 1;
                }

                // Check that going South is a part of this region, otherwise we're at a perimeter.
                if (nodeRow + 1 < numRows && map[nodeRow + 1][nodeCol] == plot) {
                    insertIntArray(&visitStack, nodeRow + 1);
                    insertIntArray(&visitStack, nodeCol);
                } else {
                    perimeter += 1;
                }

                // Check that going West is a part of this region, otherwise we're at a perimeter.
                if (nodeCol - 1 >= 0 && map[nodeRow][nodeCol - 1] == plot) {
                    insertIntArray(&visitStack, nodeRow);
                    insertIntArray(&visitStack, nodeCol - 1);
                } else {
                    perimeter += 1;
                }
            }

            totalFenceCost += area * perimeter;
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", totalFenceCost, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Instead of the sum of area * perimeter of each region, this problem is asking for the
    area * sides of each region.

    The algorithm DFS fill algorithm is the same, but instead of counting perimeters the number
    of vertices the plot has is counted, which is the same as the number of sides in the region.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Read the input into a map.
    char map[INPUT_MAX_ROWS][INPUT_MAX_COLS];
    int row = 0, col = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        col = 0;
        for (; col < lineLen; col += 1) map[row][col] = line[col];
        row += 1;
    }

    int numRows = row, numCols = col;

    // Since I don't have a true Stack data structure, treat this array as one:
    // Only add and take from the end.
    //
    // Stores the rows and cols of nodes like: [row1, col1, row2, col2, ...]
    IntArray visitStack;
    initIntArray(&visitStack, INPUT_MAX_ROWS * INPUT_MAX_COLS * 2);

    // Keep track of already visited indices. 0 = Unvisited, 1 = Visited
    int visited[numRows][numCols];
    memset(visited, 0, numRows * numCols * sizeof(int));

    // For each unvisited plot, flood it to get the area and perimeter of the region.
    char plot;
    bool northPerimeter, eastPerimeter, southPerimeter, westPerimeter;
    int totalFenceCost = 0, area, sides, nodeRow, nodeCol;
    for (int row = 0; row < numRows; row += 1) {
        for (int col = 0; col < numCols; col += 1) {
            // Already processed this region, so skip it.
            if (visited[row][col] == 1) continue;

            plot = map[row][col];
            area = 0;
            sides = 0;

            // Reset the visited stack and add the current (row, col) as the starting point
            // for the DFS.
            visitStack.numItems = 0;
            insertIntArray(&visitStack, row);
            insertIntArray(&visitStack, col);

            // Perform a DFS-based fill algorithm, searching until a map edge or different region is encountered,
            // only exploring a path if we haven't visited it before to avoid double counting area and/or perimeter.
            while (visitStack.numItems > 0) {
                // Pop the node's index off the top of the stack.
                nodeRow = visitStack.data[visitStack.numItems - 2];
                nodeCol = visitStack.data[visitStack.numItems - 1];
                visitStack.numItems -= 2;

                // Skip if we've already processed this plot.
                if (visited[nodeRow][nodeCol] == 1) continue;

                // Keep track of any found perimeters which will be used to determine the number
                // of vertices the plot might have.
                northPerimeter = false;
                eastPerimeter = false;
                southPerimeter = false;
                westPerimeter = false;

                area += 1;
                visited[nodeRow][nodeCol] = 1;

                // Check that going North is a part of this region, otherwise we're at a perimeter.
                if (nodeRow - 1 >= 0 && map[nodeRow - 1][nodeCol] == plot) {
                    insertIntArray(&visitStack, nodeRow - 1);
                    insertIntArray(&visitStack, nodeCol);
                } else {
                    northPerimeter = true;
                }

                // Check that going East is a part of this region, otherwise we're at a perimeter.
                if (nodeCol + 1 < numCols && map[nodeRow][nodeCol + 1] == plot) {
                    insertIntArray(&visitStack, nodeRow);
                    insertIntArray(&visitStack, nodeCol + 1);
                } else {
                    eastPerimeter = true;
                }

                // Check that going South is a part of this region, otherwise we're at a perimeter.
                if (nodeRow + 1 < numRows && map[nodeRow + 1][nodeCol] == plot) {
                    insertIntArray(&visitStack, nodeRow + 1);
                    insertIntArray(&visitStack, nodeCol);
                } else {
                    southPerimeter = true;
                }

                // Check that going West is a part of this region, otherwise we're at a perimeter.
                if (nodeCol - 1 >= 0 && map[nodeRow][nodeCol - 1] == plot) {
                    insertIntArray(&visitStack, nodeRow);
                    insertIntArray(&visitStack, nodeCol - 1);
                } else {
                    westPerimeter = true;
                }

                // Count "exterior" vertices, for each NE, SE, SW, NW direction, if there's a perimeter
                // in both directions, that's a vertex.
                if (northPerimeter && eastPerimeter) sides += 1;
                if (eastPerimeter && southPerimeter) sides += 1;
                if (southPerimeter && westPerimeter) sides += 1;
                if (westPerimeter && northPerimeter) sides += 1;

                // Count "interior" vertices, for each NE, SE, SW, NW direction, if there's no perimeter
                // in both directions, but the plot in the diagonal direction is of a different region,
                // that's a vertex.
                if (!northPerimeter && !eastPerimeter && (nodeRow - 1 >= 0 && nodeCol + 1 < numCols) && map[nodeRow - 1][nodeCol + 1] != plot) sides += 1;
                if (!eastPerimeter && !southPerimeter && (nodeRow + 1 < numRows && nodeCol + 1 < numCols) && map[nodeRow + 1][nodeCol + 1] != plot) sides += 1;
                if (!southPerimeter && !westPerimeter && (nodeRow + 1 < numRows && nodeCol - 1 >= 0) && map[nodeRow + 1][nodeCol - 1] != plot) sides += 1;
                if (!westPerimeter && !northPerimeter && (nodeRow - 1 >= 0 && nodeCol - 1 >= 0) && map[nodeRow - 1][nodeCol - 1] != plot) sides += 1;
            }

            totalFenceCost += area * sides;
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", totalFenceCost, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
