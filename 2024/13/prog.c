#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

#define A_COST 3
#define B_COST 1
#define SINGLE_BUTTON_LIMIT 100
#define PRIZE_INCREASE 10000000000000

void problem1(char* inputFilePath) {
    /*
    Given two buttons A and B, both of which cost a different amount of tokens to press and
    move a claw in a claw game different amount of X and Y coordinates, what's the most
    cost-efficient ways to press the buttons to move the claw to the prize coordinates.

    I realized there was probably an efficient math solution to this, but I decided to
    worry about that in part 2, where I assumed this solution would be too slow.

    For this, simply try all combinations of button presses and see if they add up
    to the right coordinates. This only works because the problem was nice enough
    to give an upper limit on button presses to get a prize.
    */
    clock_t start = clock();

    // For each claw machine, the array will have 6 consecutive entries:
    // [AX, AY, BX, BY, PRIZEX, PRIZEY, ...]
    IntArray input;
    initIntArray(&input, 1000);

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Parse the input into an array of all coordinates.
    int parserEndIdx;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        // Skip a newline.
        if (lineLen == 1) continue;

        parserEndIdx = 0;
        insertIntArray(&input, parseNumber(line, parserEndIdx, &parserEndIdx));
        insertIntArray(&input, parseNumber(line, parserEndIdx, &parserEndIdx));
    }

    fclose(inputFile);

    // For each puzzle (set of 6 in the input array), go through all combinations of A and B
    // button presses until you find one that gets you to the prize, adding the needed tokens
    // to hit those buttons to the total.
    int totalTokens = 0;
    int aX, aY, bX, bY, pX, pY, aPresses, bPresses;
    bool clawWon;
    for (int idx = 0; idx < input.numItems; idx += 6) {
        aX = input.data[idx];
        aY = input.data[idx + 1];
        bX = input.data[idx + 2];
        bY = input.data[idx + 3];
        pX = input.data[idx + 4];
        pY = input.data[idx + 5];

        aPresses = SINGLE_BUTTON_LIMIT;
        bPresses = 0;
        clawWon = false;

        // Go through all the A button presses.
        while (!clawWon && aPresses >= 0) {
            // And go through all B button presses.
            while (!clawWon && bPresses <= SINGLE_BUTTON_LIMIT) {
                // Check if the combo of button presses gets you to the prize.
                if ((aX * aPresses) + (bX * bPresses) == pX && (aY * aPresses) + (bY * bPresses) == pY) {
                    totalTokens += (A_COST * aPresses) + (B_COST * bPresses);
                    clawWon = true;
                }

                bPresses += 1;
            }

            // Working downward on A and upward on B is mostly a result of a faulty
            // assumption I made early on, but it works so I'm leaving it in.
            aPresses -= 1;
            bPresses = 0;
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", totalTokens, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Cross Multiplication:
aX * s + bX * t + pX = 0
aY * s + bY * t + pY = 0

s = (bX * pY - bY * pX) / (aX * bY - aY * bX)
t = (pX * aY - pY * aX) / (aX * bY - aY * bX)
*/

void problem2(char* inputFilePath) {
    /*
    Same as part 1 but now the prize has a huge number added to it.

    The part 1 solution is too slow, and I don't no the upper limit on button presses anymore, so we
    need to use math for this.

    After a couple of false starts looking into GCD and Euclidean algorithms, I figured it's just a set
    of linear equations with two variables, so I looked into ways to solve those.

    I went with a cross multiplication method, which ultimately gives the formula:

        aPresses = (bX * pY - bY * pX) / (aX * bY - aY * bX)
        bPresses = (pX * aY - pY * aX) / (aX * bY - aY * bX)

    From these starting equations:

        aX * aPresses + bX * bPresses + pX = 0
        aY * aPresses + bY * bPresses + pY = 0

    So, for each claw machine, plug the numbers into that equation, and double check that the results add
    up to the prize indices. If they do, add the number of tokens needed for the button presses.
    */
    clock_t start = clock();

    // For each claw machine, the array will have 6 consecutive entries:
    // [AX, AY, BX, BY, PRIZEX, PRIZEY, ...]
    LLongArray input;
    initLLongArray(&input, 1000);

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int parserEndIdx, lineNumber = 1;
    long long x, y;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        // Skip a newline.
        if (lineLen == 1) continue;

        parserEndIdx = 0;
        x = parseNumber(line, parserEndIdx, &parserEndIdx);
        y = parseNumber(line, parserEndIdx, &parserEndIdx);

        // Apply the prize increase to the prize coordinates before storing them.
        if (lineNumber == 3) {
            x += PRIZE_INCREASE;
            y += PRIZE_INCREASE;

            lineNumber = 0;
        }

        insertLLongArray(&input, x);
        insertLLongArray(&input, y);

        lineNumber += 1;
    }

    fclose(inputFile);

    // For each set of coordinates for each claw machine, solve the linear equation they  make up.
    long long totalTokens = 0;
    long long aX, aY, bX, bY, pX, pY, aPresses, bPresses;
    for (int idx = 0; idx < input.numItems; idx += 6) {
        aX = input.data[idx];
        aY = input.data[idx + 1];
        bX = input.data[idx + 2];
        bY = input.data[idx + 3];
        pX = input.data[idx + 4];
        pY = input.data[idx + 5];

        // The equations to solve for aPresses and bPresses.
        aPresses = ((bY * pX) - (bX * pY)) / ((aX * bY) - (aY * bX));
        bPresses = ((pY * aX) - (pX * aY)) / ((aX * bY) - (aY * bX));

        // Double check the numbers actually add up to the prize coordinates.
        if ((aPresses * aX) + (bPresses * bX) == pX && (aPresses * aY) + (bPresses * bY) == pY) {
            totalTokens += (aPresses * 3) + (bPresses * 1);
        }
    }

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", totalTokens, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
