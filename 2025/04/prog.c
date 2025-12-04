#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define INPUT_MAX_ROWS 150
#define INPUT_MAX_COLS 150

#define TP '@'
#define EMPTY '.'
#define MAX_TP 3

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    char tpMap[INPUT_MAX_ROWS][INPUT_MAX_COLS];
    int numRows = 0, numCols = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        for (int col = 0; col < lineLen - 1; col += 1) tpMap[numRows + 1][col + 1] = line[col];

        numCols = lineLen - 1;  // -1 get rid of the \n at the end.
        numRows += 1;
    }

    // Pad the tpMap with empty spaces on all 4 sides to avoid having to do
    // OOB checks when checking all sides of each TP roll.
    for (int r = 0; r < numRows + 2; r += 1) {
        tpMap[r][0] = EMPTY;
        tpMap[r][numCols + 1] = EMPTY;
    }
    for (int c = 0; c < numCols + 2; c += 1) {
        tpMap[0][c] = EMPTY;
        tpMap[numRows + 1][c] = EMPTY;
    }

    // Check all sides of the actual map (excluding the padding) to see if
    // it's accessible.
    int accessibleTp = 0;
    int neighborTp;
    for (int r = 1; r < numRows + 1; r += 1) {
        for (int c = 1; c < numCols + 1; c += 1) {
            if (tpMap[r][c] == EMPTY) continue;
            neighborTp = 0;

            if (tpMap[r - 1][c - 1] != EMPTY) neighborTp += 1;  // top left
            if (tpMap[r - 1][c] != EMPTY) neighborTp += 1;      // top
            if (tpMap[r - 1][c + 1] != EMPTY) neighborTp += 1;  // top right
            if (tpMap[r][c - 1] != EMPTY) neighborTp += 1;      // left
            if (tpMap[r][c + 1] != EMPTY) neighborTp += 1;      // right
            if (tpMap[r + 1][c - 1] != EMPTY) neighborTp += 1;  // bottom left
            if (tpMap[r + 1][c] != EMPTY) neighborTp += 1;      // bottom
            if (tpMap[r + 1][c + 1] != EMPTY) neighborTp += 1;  // bottom right

            if (neighborTp <= MAX_TP) accessibleTp += 1;
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", accessibleTp, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Do problem1 in a loop, actively removing the accessible TP, only stopping when no more TP
    can be removed.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    char tpMap[INPUT_MAX_ROWS][INPUT_MAX_COLS];
    int numRows = 0, numCols = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        for (int col = 0; col < lineLen - 1; col += 1) tpMap[numRows + 1][col + 1] = line[col];

        numCols = lineLen - 1;  // -1 get rid of the \n at the end.
        numRows += 1;
    }

    for (int r = 0; r < numRows + 2; r += 1) {
        tpMap[r][0] = EMPTY;
        tpMap[r][numCols + 1] = EMPTY;
    }
    for (int c = 0; c < numCols + 2; c += 1) {
        tpMap[0][c] = EMPTY;
        tpMap[numRows + 1][c] = EMPTY;
    }

    bool removedSomeTp = true;
    int accessibleTp = 0;
    int neighborTp;
    while (removedSomeTp) {
        removedSomeTp = false;
        for (int r = 1; r < numRows + 1; r += 1) {
            for (int c = 1; c < numCols + 1; c += 1) {
                if (tpMap[r][c] == EMPTY) continue;
                neighborTp = 0;

                if (tpMap[r - 1][c - 1] != EMPTY) neighborTp += 1;  // top left
                if (tpMap[r - 1][c] != EMPTY) neighborTp += 1;      // top
                if (tpMap[r - 1][c + 1] != EMPTY) neighborTp += 1;  // top right
                if (tpMap[r][c - 1] != EMPTY) neighborTp += 1;      // left
                if (tpMap[r][c + 1] != EMPTY) neighborTp += 1;      // right
                if (tpMap[r + 1][c - 1] != EMPTY) neighborTp += 1;  // bottom left
                if (tpMap[r + 1][c] != EMPTY) neighborTp += 1;      // bottom
                if (tpMap[r + 1][c + 1] != EMPTY) neighborTp += 1;  // bottom right

                if (neighborTp <= MAX_TP) {
                    accessibleTp += 1;
                    removedSomeTp = true;
                    tpMap[r][c] = EMPTY;  // remove the roll.
                }
            }
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", accessibleTp, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
