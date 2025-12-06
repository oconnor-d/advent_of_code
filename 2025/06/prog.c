#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/string.c"

#define MAX_ROWS 10
#define MAX_COLS 5000

void problem1(char* inputFilePath) {
    clock_t start = clock();

    // 1. Get the list of numbers and the operators.
    int numbers[MAX_ROWS][MAX_COLS];
    char operands[MAX_COLS];

    int rows = 0, cols = 0;
    int endIdx;
    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        endIdx = 0;
        cols = 0;
        if (line[0] == '*' || line[0] == '+') {
            for (int idx = 0; idx < lineLen - 1; idx += 1) {
                if (line[idx] != ' ') {
                    operands[cols] = line[idx];
                    cols += 1;
                }
            }
        } else {
            while (endIdx >= 0) {
                numbers[rows][cols] = parseNumber(line, endIdx, &endIdx);
                cols += 1;
            }
            rows += 1;
        }
    }

    // 2. Do the math.
    long number, total = 0;
    for (int c = 0; c < cols; c += 1) {
        number = operands[c] == '+' ? 0 : 1;
        for (int r = 0; r < rows; r += 1) {
            if (operands[c] == '+')
                number += numbers[r][c];
            else
                number *= numbers[r][c];
        }
        total += number;
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %ld [%.2fms]\n", total, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    // 1. Parse the input into a 2d char array for easier processing.
    char input[MAX_ROWS][MAX_COLS];
    int rows = 0, cols = 0;

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        for (int idx = 0; idx < lineLen - 1; idx += 1) input[rows][idx] = line[idx];
        rows += 1;
        cols = lineLen - 1;
    }

    int numbers[rows];

    // 2. Parse the numbers and add them up once we hit the operand.
    char operand;
    long total = 0;
    int idx = 0;
    for (int c = cols - 1; c >= 0; c -= 1) {
        if (input[0][c] == ' ' && input[rows - 2][c] == ' ') continue;
        operand = input[rows - 1][c];

        // Build up the number in this column.
        int num = 0, pos = 1;
        for (int r = rows - 2; r >= 0; r -= 1) {
            if (input[r][c] == ' ') continue;
            num += (input[r][c] - '0') * pos;
            pos *= 10;
        }

        // Keep track of it.
        numbers[idx] = num;
        idx += 1;

        // Once we've found the operand, we're good to do the math for the set.
        if (operand != ' ') {
            long base = operand == '+' ? 0 : 1;
            for (int i = 0; i < idx; i += 1) {
                if (operand == '+')
                    base += numbers[i];
                else
                    base *= numbers[i];
            }
            total += base;
            // Reset
            idx = 0;
            operand = ' ';
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %ld [%.2fms]\n", total, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
