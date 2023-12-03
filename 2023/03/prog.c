#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

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

    
    int inputRows = 0, inputCols = 0;
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        inputCols = lineLength;
        inputRows += 1;
    }
    fseek(inputFile, 0, SEEK_SET);

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
    int row = 0, numSymbols = 0;
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        
        int idx = 0;
        while (idx < lineLength) {
            if (isdigit(line[idx])) {
                char numberString[lineLength + 1];
                int numberIdx = 0;

                // TODO: can probably just get the index of the end character or next . and copy it over in a whole chunk.
                while (isdigit(line[idx]) && idx < lineLength) {
                    numberString[numberIdx] = line[idx];
                    numberIdx += 1;
                    idx += 1;
                }
                numberString[numberIdx] = '\0';
                int number = atoi(numberString);

                while (numberIdx > 0) {
                    numbers[row][idx - numberIdx] = number;
                    numberIdx -= 1;
                }
            } else if (line[idx] != '.' && line[idx] != '\n') {
                symbolPositions[numSymbols][0] = row;
                symbolPositions[numSymbols][1] = idx;
                symbolPositions[numSymbols][2] = line[idx] == GEAR_SYMBOL;
                idx += 1;
                numSymbols += 1;
            } else {
                idx += 1;
            }

        }

        row += 1;
    }

    int symbolRow, symbolCol, symbolIsGear, symbolGearRatio, numGearParts;
    for (int symbolIdx = 0; symbolIdx < numSymbols; symbolIdx += 1) {
        symbolRow = symbolPositions[symbolIdx][0];
        symbolCol = symbolPositions[symbolIdx][1];
        symbolIsGear = symbolPositions[symbolIdx][2];
        symbolGearRatio = 1;
        numGearParts = 0;

        /*
            Positions to check for a symbol (not on the edge of the board):

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
    double timeElapsedInSeconds = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Problem 01: %d [%fs]\n", partsTotal, timeElapsedInSeconds);
    printf("Problem 02: %d [%fs]\n", gearRatioTotal, timeElapsedInSeconds);
}


int main() {
    problem1And2();
}
