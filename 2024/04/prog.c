#include <stdio.h>
#include <string.h>
#include <time.h>

#define INPUT_MAX_ROWS 150
#define INPUT_MAX_COLS 150

void problem1(char* inputFilePath) {
    /*
    This problem is asking us to find the number of times XMAS shows up in the input,
    either forward, backwards, up, down, diagonal.

    Load the input into a 2d array and check each direction (assuming the direction isn't out of bounds)
    for XMAS whenever we come across an X.
    */
    clock_t start = clock();

    // Assume each row has the same number of columns.
    FILE* inputFile = fopen(inputFilePath, "r");

    char input[(INPUT_MAX_ROWS * INPUT_MAX_COLS) + 1];
    size_t fileLen = fread(input, sizeof(char), INPUT_MAX_ROWS * INPUT_MAX_COLS, inputFile);

    fclose(inputFile);

    char wordSearch[INPUT_MAX_ROWS][INPUT_MAX_COLS];

    // Load the input into a 2D array for easier processing.
    int row = 0, col = 0;
    for (int idx = 0; idx < fileLen; idx += 1) {
        if (input[idx] == '\n') {
            row += 1;
            col = 0;
        } else {
            wordSearch[row][col] = input[idx];
            col += 1;
        }
    }

    int numRows = row + 1;
    int numCols = col;
    int numXmas = 0;

    // Loop through the input until we find an X, then search all directions for the
    // rest of the word.
    for (int row = 0; row < numRows; row += 1) {
        for (int col = 0; col < numCols; col += 1) {
            if (wordSearch[row][col] != 'X') continue;

            // We've already found the X, now we check for the rest in each possible direction,
            // first checking the direction won't take us out of bounds before checking that
            // the right characters are there.

            // N
            if (
                row >= 3 &&
                wordSearch[row - 1][col] == 'M' &&
                wordSearch[row - 2][col] == 'A' &&
                wordSearch[row - 3][col] == 'S') {
                numXmas += 1;
            }

            // NE
            if (
                row >= 3 && col < numCols - 3 &&
                wordSearch[row - 1][col + 1] == 'M' &&
                wordSearch[row - 2][col + 2] == 'A' &&
                wordSearch[row - 3][col + 3] == 'S') {
                numXmas += 1;
            }

            // E
            if (
                col < numCols - 3 &&
                wordSearch[row][col + 1] == 'M' &&
                wordSearch[row][col + 2] == 'A' &&
                wordSearch[row][col + 3] == 'S') {
                numXmas += 1;
            }

            // SE
            if (
                row < numRows - 3 && col < numCols - 3 &&
                wordSearch[row + 1][col + 1] == 'M' &&
                wordSearch[row + 2][col + 2] == 'A' &&
                wordSearch[row + 3][col + 3] == 'S') {
                numXmas += 1;
            }

            // S
            if (
                row < numRows - 3 &&
                wordSearch[row + 1][col] == 'M' &&
                wordSearch[row + 2][col] == 'A' &&
                wordSearch[row + 3][col] == 'S') {
                numXmas += 1;
            }

            // SW
            if (
                row < numRows - 3 && col >= 3 &&
                wordSearch[row + 1][col - 1] == 'M' &&
                wordSearch[row + 2][col - 2] == 'A' &&
                wordSearch[row + 3][col - 3] == 'S') {
                numXmas += 1;
            }

            // W
            if (
                col >= 3 &&
                wordSearch[row][col - 1] == 'M' &&
                wordSearch[row][col - 2] == 'A' &&
                wordSearch[row][col - 3] == 'S') {
                numXmas += 1;
            }

            // NW
            if (
                row >= 3 && col >= 3 &&
                wordSearch[row - 1][col - 1] == 'M' &&
                wordSearch[row - 2][col - 2] == 'A' &&
                wordSearch[row - 3][col - 3] == 'S') {
                numXmas += 1;
            }
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", numXmas, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Instead of looking for an XMAS, this part looks for an X-MAS, or:

        M . M
        . A .
        S . S
    
    which is an X made out of two MASes. The above X-MAS is valid even when rotated any number
    of times.

    Similar to part 1, we load the input into a 2D array, and check that an X-MAS would be in bounds
    and then for the appropriate characters surrounding any A we find.
    */
    clock_t start = clock();

    // Assume each row has the same number of columns.
    FILE* inputFile = fopen(inputFilePath, "r");

    char input[(INPUT_MAX_ROWS * INPUT_MAX_COLS) + 1];
    size_t fileLen = fread(input, sizeof(char), INPUT_MAX_ROWS * INPUT_MAX_COLS, inputFile);

    fclose(inputFile);

    char wordSearch[INPUT_MAX_ROWS][INPUT_MAX_COLS];

    // Load the input into a 2D array for easier processing.
    int row = 0, col = 0;
    for (int idx = 0; idx < fileLen; idx += 1) {
        if (input[idx] == '\n') {
            row += 1;
            col = 0;
        } else {
            wordSearch[row][col] = input[idx];
            col += 1;
        }
    }

    int numRows = row + 1;
    int numCols = col;
    int numXmas = 0;

    // Loop through the input until we find an A, then search all the corners to see
    // if they form an X-MAS.
    for (int row = 0; row < numRows; row += 1) {
        for (int col = 0; col < numCols; col += 1) {
            if (wordSearch[row][col] != 'A') continue;

            // We've already found the A, now we check for the X-MAS, first checking
            // it can be in bounds.

            // An X-MAS is guaranteed to be in-bounds if the A isn't on the edges of the word seach.
            if (row == 0 || row == numRows - 1 || col == 0 || col == numCols - 1) continue;

            // Now that we're in bounds, check that both tops are M . M and both tops are S . S, or
            // vice versa, or both lefts are M . M and both rights are S . S, or vice versa.

            // M . M top, S . S bottom check.
            if (wordSearch[row - 1][col - 1] == 'M' && wordSearch[row - 1][col + 1] == 'M' &&
                wordSearch[row + 1][col - 1] == 'S' && wordSearch[row + 1][col + 1] == 'S') {
                numXmas += 1;
            }
            // S . S top, M . M bottom check.
            else if (wordSearch[row - 1][col - 1] == 'S' && wordSearch[row - 1][col + 1] == 'S' &&
                     wordSearch[row + 1][col - 1] == 'M' && wordSearch[row + 1][col + 1] == 'M') {
                numXmas += 1;
            }
            // M . M left, S . S right check.
            else if (wordSearch[row - 1][col - 1] == 'M' && wordSearch[row - 1][col + 1] == 'S' &&
                     wordSearch[row + 1][col - 1] == 'M' && wordSearch[row + 1][col + 1] == 'S') {
                numXmas += 1;
            }
            // S . S left, M . M right check.
            else if (wordSearch[row - 1][col - 1] == 'S' && wordSearch[row - 1][col + 1] == 'M' &&
                     wordSearch[row + 1][col - 1] == 'S' && wordSearch[row + 1][col + 1] == 'M') {
                numXmas += 1;
            }
        }
    }

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", numXmas, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
