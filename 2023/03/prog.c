#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../../utils/string.c"

int EMPTY_NUMBER = 0;
char GEAR_SYMBOL = '*';


void problem1And2() {
    clock_t start = clock();

    int partsTotal = 0;
    int gearRatioTotal = 0;

    FILE* inputFile = fopen("2023/03/input.txt", "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLength;

    // Count the number of rows and cols in the input.
    int inputRows = 0, inputCols = 0;
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        inputCols = lineLength;
        inputRows += 1;
    }
    fseek(inputFile, 0, SEEK_SET);

    // Numbers will hold all the numbers found in the input, in the same row and col as the numbers appear in the input.
    // Numbers that take up multiple characters in the input (e.g. 123 in '..123..') will appear in the numbers array in
    // the same place as each digit of the number (e.g. '..123..' -> [X, X, 123, 123, 123, X, X]).
    int numbers[inputRows][inputCols];
    for (int r = 0; r < inputRows; r += 1) {
        for (int c = 0; c < inputCols; c += 1) {
            numbers[r][c] = EMPTY_NUMBER;
        }
    }

    // inputRows * inputCols = max number of symbols a puzzle with nothing but symbols can have.
    // A symbolPosition is of the form: [symbolRow, symbolCol, isGear]. Where isGear of 1 means the
    // symbol is a gear, and an isGear of 0 means otherwise.
    int symbolPositions[inputRows * inputCols][3];

    // Parse the input file into the number and symbol position arrays.
    int row = 0, numSymbols = 0;
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        int idx = 0;
        while (idx < lineLength) {
            if (isdigit(line[idx])) {
                // Found a number.
                int startIdx, endIdx;
                int number = parseFirstNumber(line + idx, &startIdx, &endIdx);
                int numberLength = endIdx - startIdx;

                for (int nIdx = 0; nIdx < numberLength; nIdx += 1) {
                    numbers[row][idx + nIdx] = number;
                }

                idx += numberLength;
            } else if (line[idx] != '.' && line[idx] != '\n') {
                // Found a symbol.
                symbolPositions[numSymbols][0] = row;
                symbolPositions[numSymbols][1] = idx;
                symbolPositions[numSymbols][2] = line[idx] == GEAR_SYMBOL;
                idx += 1;
                numSymbols += 1;
            } else {
                // Found a '.', which gets ignored.
                idx += 1;
            }
        }

        row += 1;
    }

    // For each symbol, add up the surrounding numbers.
    int symbolRow, symbolCol, symbolIsGear, symbolGearRatio, numGearParts;
    for (int symbolIdx = 0; symbolIdx < numSymbols; symbolIdx += 1) {
        symbolRow = symbolPositions[symbolIdx][0];
        symbolCol = symbolPositions[symbolIdx][1];
        symbolIsGear = symbolPositions[symbolIdx][2];
        symbolGearRatio = 1;
        numGearParts = 0;

        /*
            Positions to check for a symbol (that's not on an edge of the board):

            (-1, -1), (-1, 0), (-1, 1)
            (0, -1) , X      , (0, 1)
            (1, -1) , (1, 0) , (1, 1) 
        */

        // Check above row for numbers.
        int prevNumber = EMPTY_NUMBER;
        if (symbolRow > 0) {
            if (symbolCol > 0 && numbers[symbolRow -1][symbolCol -1] != EMPTY_NUMBER) {
                partsTotal += numbers[symbolRow - 1][symbolCol - 1];
                prevNumber = numbers[symbolRow - 1][symbolCol - 1];

                if (symbolIsGear) {
                    symbolGearRatio *= numbers[symbolRow - 1][symbolCol - 1];
                    numGearParts += 1;
                }
            }

            if (numbers[symbolRow - 1][symbolCol] != EMPTY_NUMBER && numbers[symbolRow - 1][symbolCol] != prevNumber) {
                partsTotal += numbers[symbolRow - 1][symbolCol];
                prevNumber = numbers[symbolRow - 1][symbolCol];

                if (symbolIsGear) {
                    symbolGearRatio *= numbers[symbolRow - 1][symbolCol];
                    numGearParts += 1;
                }
            }

            if (symbolCol < inputCols - 1 && numbers[symbolRow - 1][symbolCol + 1] != EMPTY_NUMBER && numbers[symbolRow - 1][symbolCol + 1] != prevNumber) {
                partsTotal += numbers[symbolRow - 1][symbolCol + 1];

                if (symbolIsGear) {
                    symbolGearRatio *= numbers[symbolRow - 1][symbolCol + 1];
                    numGearParts += 1;
                }
            }
        }

        // Check left and right cols for numbers.
        if (symbolCol > 0 && numbers[symbolRow][symbolCol - 1] != EMPTY_NUMBER) {
            partsTotal += numbers[symbolRow][symbolCol - 1];

            if (symbolIsGear) {
                symbolGearRatio *= numbers[symbolRow][symbolCol - 1];
                numGearParts += 1;
            }
        }

        if (symbolCol < inputCols - 1 && numbers[symbolRow][symbolCol + 1] != EMPTY_NUMBER) {
            partsTotal += numbers[symbolRow][symbolCol + 1];

            if (symbolIsGear) {
                symbolGearRatio *= numbers[symbolRow][symbolCol + 1];
                numGearParts += 1;
            }
        }

        // Check below row for numbers.
        prevNumber = EMPTY_NUMBER;
        if (symbolRow < inputRows - 1) {
            if (symbolCol > 0 && numbers[symbolRow + 1][symbolCol -1] != EMPTY_NUMBER) {
                partsTotal += numbers[symbolRow + 1][symbolCol - 1];
                prevNumber = numbers[symbolRow + 1][symbolCol - 1];

                if (symbolIsGear) {
                    symbolGearRatio *= numbers[symbolRow + 1][symbolCol - 1];
                    numGearParts += 1;
                }
            }

            if (numbers[symbolRow + 1][symbolCol] != EMPTY_NUMBER && numbers[symbolRow + 1][symbolCol] != prevNumber) {
                partsTotal += numbers[symbolRow + 1][symbolCol];
                prevNumber = numbers[symbolRow + 1][symbolCol];

                if (symbolIsGear) {
                    symbolGearRatio *= numbers[symbolRow + 1][symbolCol];
                    numGearParts += 1;
                }
            }

            if (symbolCol < inputCols - 1 && numbers[symbolRow + 1][symbolCol + 1] != EMPTY_NUMBER && numbers[symbolRow + 1][symbolCol + 1] != prevNumber) {
                partsTotal += numbers[symbolRow + 1][symbolCol + 1];

                if (symbolIsGear) {
                    symbolGearRatio *= numbers[symbolRow + 1][symbolCol + 1];
                    numGearParts += 1;
                }
            }
        }
    
        if (numGearParts > 1) {
            gearRatioTotal += symbolGearRatio;
        }
    }

    clock_t end = clock();
    double timeElapsedInMilliSeconds = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    printf("Problem 01: %d [%.2fms]\n", partsTotal, timeElapsedInMilliSeconds);
    printf("Problem 02: %d [%.2fms]\n", gearRatioTotal, timeElapsedInMilliSeconds);
}


int main() {
    problem1And2();

    return 0;
}
