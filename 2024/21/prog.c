#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"

#define NUMERICAL_A 10
#define DIRECTIONAL_A 4
#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3

/*
Numeric Keypad
+---+---+---+
| 7 | 8 | 9 |
+---+---+---+
| 4 | 5 | 6 |
+---+---+---+
| 1 | 2 | 3 |
+---+---+---+
    | 0 | A |
    +---+---+

Directional Keypad
    +---+---+
    | ^ | A |
+---+---+---+
| < | v | > |
+---+---+---+

- One directional keypad that you are using.
- Two directional keypads that robots are using.
- One numeric keypad (on a door) that a robot is using.

So, to press 029A on the numeric keypad, you need to command
a robot to command another robot to command the final robot.
*/

void printOutput(IntArray* presses) {
    for (int idx = 0; idx < presses->numItems; idx += 1) {
        if (presses->data[idx] == DIRECTIONAL_A)
            printf("A");
        else if (presses->data[idx] == UP)
            printf("^");
        else if (presses->data[idx] == LEFT)
            printf("<");
        else if (presses->data[idx] == DOWN)
            printf("v");
        else
            printf(">");
    }
    printf("\n");
}

// Button Number -> (row, col)
int buttonLocations[11][2] = {
    {3, 1},
    {2, 0},
    {2, 1},
    {2, 2},
    {1, 0},
    {1, 1},
    {1, 2},
    {0, 0},
    {0, 1},
    {0, 2},
    {3, 2},
};

int directionLocations[5][2] = {
    {0, 1},
    {1, 0},
    {1, 1},
    {1, 2},
    {0, 2},
};

void solveNumeric(int buttons[4], IntArray* directions) {
    int row = buttonLocations[NUMERICAL_A][0], col = buttonLocations[NUMERICAL_A][1];

    int targetRow, targetCol;
    for (int idx = 0; idx < 4; idx += 1) {
        targetRow = buttonLocations[buttons[idx]][0];
        targetCol = buttonLocations[buttons[idx]][1];

        if (row < 3 || (row == 3 && targetCol > 0)) {
            while (targetCol < col) {
                insertIntArray(directions, LEFT);
                col -= 1;
            }
        }
        if (col > 0 || (col == 0 && targetRow < 3)) {
            while (targetRow > row) {
                insertIntArray(directions, DOWN);
                row += 1;
            }
        }

        while (targetCol > col) {
            insertIntArray(directions, RIGHT);
            col += 1;
        }

        while (targetRow < row) {
            insertIntArray(directions, UP);
            row -= 1;
        }

        while (targetCol < col) {
            insertIntArray(directions, LEFT);
            col -= 1;
        }

        while (targetRow > row) {
            insertIntArray(directions, DOWN);
            row += 1;
        }

        insertIntArray(directions, DIRECTIONAL_A);
    }
}

void solveDirectional(IntArray* buttons, IntArray* directions) {
    int row = directionLocations[DIRECTIONAL_A][0], col = directionLocations[DIRECTIONAL_A][1];

    int targetRow, targetCol;
    for (int idx = 0; idx < buttons->numItems; idx += 1) {
        targetRow = directionLocations[buttons->data[idx]][0];
        targetCol = directionLocations[buttons->data[idx]][1];

        // Below the empty space.
        if (row == 1 && col == 0 && targetRow == 0) {
            while (targetCol > col) {
                insertIntArray(directions, RIGHT);
                col += 1;
            }
        }

        while (targetCol < col) {
            insertIntArray(directions, LEFT);
            col -= 1;
        }

        while (targetCol > col) {
            insertIntArray(directions, RIGHT);
            col += 1;
        }

        while (targetRow < row) {
            insertIntArray(directions, UP);
            row -= 1;
        }

        while (targetRow > row) {
            insertIntArray(directions, DOWN);
            row += 1;
        }

        insertIntArray(directions, DIRECTIONAL_A);
    }
}

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    IntArray directions1, directions2, directions3;

    int buttons[4];
    int complexity = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        initIntArray(&directions1, 1024);
        initIntArray(&directions2, 1024);
        initIntArray(&directions3, 1024);

        buttons[0] = line[0] - '0';
        buttons[1] = line[1] - '0';
        buttons[2] = line[2] - '0';
        buttons[3] = NUMERICAL_A;

        solveNumeric(buttons, &directions1);
        solveDirectional(&directions1, &directions2);
        solveDirectional(&directions2, &directions3);

        complexity += (buttons[0] * 100 + buttons[1] * 10 + buttons[2]) * directions3.numItems;

        freeIntArray(&directions1);
        freeIntArray(&directions2);
        freeIntArray(&directions3);
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", complexity, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    IntArray test1, test2;
    IntArray *input, *output, *temp;

    int buttons[4];
    long long complexity = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        initIntArray(&test1, 1024);
        initIntArray(&test2, 1024);

        input = &test1;
        output = &test2;

        buttons[0] = line[0] - '0';
        buttons[1] = line[1] - '0';
        buttons[2] = line[2] - '0';
        buttons[3] = NUMERICAL_A;

        solveNumeric(buttons, input);

        int robotNum = 0;
        while (robotNum < 10) {
            // printf("%d\n", robotNum);
            solveDirectional(input, output);

            // printf("%zu\n", output->numItems);
            if (robotNum != 9) {
                freeIntArray(input);
                initIntArray(input, 1024);

                temp = input;
                input = output;
                output = temp;
            }

            robotNum += 1;
        };

        complexity += (buttons[0] * 100 + buttons[1] * 10 + buttons[2]) * output->numItems;

        freeIntArray(input);
        freeIntArray(output);
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", complexity, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
