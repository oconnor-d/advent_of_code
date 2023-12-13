#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "../../utils/array.c"
#include "../../utils/string.c"


char STARTING_SYMBOL = 'S';


int checkInBounds(int position, int numPositions) {
    if (position >= numPositions || position < 0) {
        return -1;
    }

    return position;
}


// https://en.wikipedia.org/wiki/Shoelace_formula
// Gets the area of the polygon described by the given set of vertices. (vertices = [x1, y1, x2, y2, ...])
int shoelaceArea(IntArray* vertices) {
    int area = 0;

    int x1TimesY2, y1TimesX2;
    for (int idx = 0; idx < vertices->numItems; idx += 4) {
        x1TimesY2 = vertices->data[idx] * vertices->data[idx + 3];
        y1TimesX2 = vertices->data[idx + 1] * vertices->data[idx + 2];

        area += y1TimesX2 - x1TimesY2;
    }

    return abs(area);
}


// https://en.wikipedia.org/wiki/Pick%27s_theorem
// Give the area of the polygon and the number of integer points on it's boundary, we can get the number of interior
// points by modifying Picks Theorem, which normally gets the area from the number of interior points + boundary points.
//
// i = # interior points
// b = # boundary points
//
// A = i + b/2 - 1
// A + 1 = i + b/2
// A + 1 - b/2 = i
int picksTheoremNumInteriorPoints(int area, int b) {
    return area + 1 - (b / 2);
}


void getNeighboringCoordinates(char pipe, int pipeRow, int pipeCol, int numRows, int numCols, int* coordinates) {
    int newRow1 = pipeRow, newCol1 = pipeCol, newRow2 = pipeRow, newCol2 = pipeCol;

    if (pipe == '|') {
        newRow1 = checkInBounds(pipeRow - 1, numRows);
        newCol1 = pipeCol;

        newRow2 = checkInBounds(pipeRow + 1, numRows);
        newCol2 = pipeCol;
    } else if (pipe == '-') {
        newRow1 = pipeRow;
        newCol1 = checkInBounds(pipeCol - 1, numCols);

        newRow2 = pipeRow;
        newCol2 = checkInBounds(pipeCol + 1, numCols);
    } else if (pipe == 'L') {
        newRow1 = checkInBounds(pipeRow - 1, numRows);
        newCol1 = pipeCol;

        newRow2 = pipeRow;
        newCol2 = checkInBounds(pipeCol + 1, numCols);
    } else if (pipe == 'J') {
        newRow1 = checkInBounds(pipeRow - 1, numRows);
        newCol1 = pipeCol;

        newRow2 = pipeRow;
        newCol2 = checkInBounds(pipeCol - 1, numCols);
    } else if (pipe == '7') {
        newRow1 = checkInBounds(pipeRow + 1, numRows);
        newCol1 = pipeCol;

        newRow2 = pipeRow;
        newCol2 = checkInBounds(pipeCol - 1, numCols);
    } else if (pipe == 'F') {
        newRow1 = checkInBounds(pipeRow + 1, numRows);
        newCol1 = pipeCol;

        newRow2 = pipeRow;
        newCol2 = checkInBounds(pipeCol + 1, numCols);
    }

    coordinates[0] = newRow1;
    coordinates[1] = newCol1;
    coordinates[2] = newRow2;
    coordinates[3] = newCol2;
}


void problem1() {
    clock_t start = clock();
    
    FILE* inputFile = fopen("2023/10/input.txt", "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLength;

    // Get the 2D array dimensions.
    int pipeRows = 0, pipeCols = 0;
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        pipeCols = lineLength;
        pipeRows += 1;
    }
    fseek(inputFile, 0, SEEK_SET);

    // Read the pipes into a 2D array, and get the starting point coordinates.
    char pipes[pipeRows][pipeCols];
    int startRow, startCol, currentRow = 0;
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        for (int currentCol = 0; currentCol < lineLength; currentCol += 1) {
            if (line[currentCol] == STARTING_SYMBOL) {
                startRow = currentRow;
                startCol = currentCol;
            }
            pipes[currentRow][currentCol] = line[currentCol];
        }

        currentRow += 1;
    }
    fclose(inputFile);

    // Get the first set of connecting pipes to the starting point.
    int pipe1Row, pipe1Col, pipe2Row, pipe2Col;
    bool pipe1Found = false;
    // N
    if (checkInBounds(startRow - 1, pipeRows) != -1 && (pipes[startRow - 1][startCol] == '|' || pipes[startRow - 1][startCol] == 'F' || pipes[startRow - 1][startCol] == '7')) {
        pipe1Row = startRow - 1;
        pipe1Col = startCol;
        pipe1Found = true;
    }
    // E
    if (checkInBounds(startCol + 1, pipeCols) != -1 && (pipes[startRow][startCol + 1] == '-' || pipes[startRow][startCol + 1] == '7' || pipes[startRow][startCol + 1] == 'J')) {
        if (!pipe1Found) {
            pipe1Row = startRow;
            pipe1Col = startCol + 1;
            pipe1Found = true;
        } else {
            pipe2Row = startRow;
            pipe2Col = startCol + 1;
        }
    }
    // S
    if (checkInBounds(startRow + 1, pipeRows) != -1 && (pipes[startRow + 1][startCol] == '|' || pipes[startRow + 1][startCol] == 'L' || pipes[startRow + 1][startCol] == 'J')) {
        if (!pipe1Found) {
            pipe1Row = startRow + 1;
            pipe1Col = startCol;
            pipe1Found = true;
        } else {
            pipe2Row = startRow + 1;
            pipe2Col = startCol;
        }
    }
    // W
    if (checkInBounds(startCol - 1, pipeCols) != -1 && (pipes[startRow][startCol - 1] == '-' || pipes[startRow][startCol - 1] == 'F' || pipes[startRow][startCol - 1] == 'L')) {
        // We must have already found the first pipe, so this must be the second.
        pipe2Row = startRow;
        pipe2Col = startCol - 1;
    }
 
    // "Destroy" the starting position since that's how I check not to revisit the pipe below.
    pipes[startRow][startCol] = 'X';

    // Since the pipe is a loop, the furthest distance from the starting point is the same
    // when traveling in both directions from the starting pipe.
    int pipeDistance = 0;
    int coords[4];
    while (!(pipe1Row == pipe2Row && pipe1Col == pipe2Col)) {
        pipeDistance += 1;

        getNeighboringCoordinates(pipes[pipe1Row][pipe1Col], pipe1Row, pipe1Col, pipeRows, pipeCols, coords);
        // "Destroy" previously visited pipes for easy checking if we already visited it
        pipes[pipe1Row][pipe1Col] = 'X';
        if (pipes[coords[0]][coords[1]] != 'X') {
            pipe1Row = coords[0];
            pipe1Col = coords[1];
        } else {
            pipe1Row = coords[2];
            pipe1Col = coords[3];
        }

        getNeighboringCoordinates(pipes[pipe2Row][pipe2Col], pipe2Row, pipe2Col, pipeRows, pipeCols, coords);
        // "Destroy" previously visited pipes for easy checking if we already visited it
        pipes[pipe2Row][pipe2Col] = 'X';
        if (pipes[coords[0]][coords[1]] != 'X') {
            pipe2Row = coords[0];
            pipe2Col = coords[1];
        } else {
            pipe2Row = coords[2];
            pipe2Col = coords[3];
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", pipeDistance + 1, (double)(end - start) / CLOCKS_PER_SEC * 1000);\
}


void problem2() {
    clock_t start = clock();
    
    FILE* inputFile = fopen("2023/10/input.txt", "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLength;


    // Get the 2D array dimensions.
    int pipeRows = 0, pipeCols = 0;
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        pipeCols = lineLength;
        pipeRows += 1;
    }
    fseek(inputFile, 0, SEEK_SET);

    // Read the pipes into a 2D array, and get the starting point coordinates.
    char pipes[pipeRows][pipeCols];
    int startRow, startCol, currentRow = 0;
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        for (int currentCol = 0; currentCol < lineLength; currentCol += 1) {
            if (line[currentCol] == STARTING_SYMBOL) {
                startRow = currentRow;
                startCol = currentCol;
            }
            pipes[currentRow][currentCol] = line[currentCol];
        }

        currentRow += 1;
    }
    fclose(inputFile);

    // Get the first connecting pipe to the starting point. We only need to check the first three neighboring coords since there
    // are two connecting points (Pigeonhole Principle!)
    int pipeRow = -1, pipeCol = -1;
    if (checkInBounds(startRow - 1, pipeRows) != -1 && (pipes[startRow - 1][startCol] == '|' || pipes[startRow - 1][startCol] == 'F' || pipes[startRow - 1][startCol] == '7')) {
        pipeRow = startRow - 1;
        pipeCol = startCol;
    } else if (checkInBounds(startCol + 1, pipeCols) != -1 && (pipes[startRow][startCol + 1] == '-' || pipes[startRow][startCol + 1] == '7' || pipes[startRow][startCol + 1] == 'J')) {
        pipeRow = startRow;
        pipeCol = startCol + 1;
    } else if (checkInBounds(startRow + 1, pipeRows) != -1 && (pipes[startRow + 1][startCol] == '|' || pipes[startRow + 1][startCol] == 'L' || pipes[startRow + 1][startCol] == 'J')) {
        pipeRow = startRow + 1;
        pipeCol = startCol;
    }
 
    // "Destroy" the starting position since that's how I check not to revisit the pipe below.
    pipes[startRow][startCol] = 'X';

    // Since the pipe is a loop, the furthest distance from the starting point is the same
    // when traveling in both directions from the starting pipe.
    int insideTiles = 0;
    int coords[4];

    IntArray vertices;
    initIntArray(&vertices, 32);

    bool startingPipeIsVertex = false;
    // Check up and down symbols
    if (checkInBounds(startRow - 1, pipeRows) != -1 && checkInBounds(startRow + 1, pipeRows) && (pipes[startRow - 1][startCol] != '|' || pipes[startRow + 1][startCol] != '|')) {
        startingPipeIsVertex = true;
    } else if (checkInBounds(startCol - 1, pipeCols) != -1 && checkInBounds(startCol + 1, pipeCols) && (pipes[startRow][startCol - 1] != '-' || pipes[startRow][startCol + 1] != '-')) {
        startingPipeIsVertex = true;
    }

    // Must be a vertex.
    if (startingPipeIsVertex) {
        insertIntArray(&vertices, startRow);
        insertIntArray(&vertices, startCol);
    }

    int boundaryPoints = 1;
    while (pipes[pipeRow][pipeCol] != 'X') {
        // Must be a vertex.
        if (pipes[pipeRow][pipeCol] != '-' && pipes[pipeRow][pipeCol] != '|') {
            insertIntArray(&vertices, pipeRow);
            insertIntArray(&vertices, pipeCol);
        }
        getNeighboringCoordinates(pipes[pipeRow][pipeCol], pipeRow, pipeCol, pipeRows, pipeCols, coords);
        // "Destroy" previously visited pipes for easy checking if we already visited it
        pipes[pipeRow][pipeCol] = 'X';
        if (pipes[coords[0]][coords[1]] != 'X') {
            pipeRow = coords[0];
            pipeCol = coords[1];
        } else {
            pipeRow = coords[2];
            pipeCol = coords[3];
        }

        boundaryPoints += 1;
    }

    // We use the Shoelace Formula to get the area from the vertices, and then
    // Pick's Theorem to get the number of interior points from the area and
    // number of points on the boundary.
    insideTiles = picksTheoremNumInteriorPoints(shoelaceArea(&vertices), boundaryPoints);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", insideTiles, (double)(end - start) / CLOCKS_PER_SEC * 1000);\
}


int main() {
    problem1();
    problem2();

    return 0;
}
