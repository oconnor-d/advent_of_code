#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

#define INPUT_SIZE 500
#define SECONDS 100

void problem1(char* inputFilePath) {
    /*
    The problem gives us a set of robot starting positions and velocities, and asks for their
    locations after moving a certain number of seconds. If the robots go off bounds, they
    warp to the other side. After that, split the robots into quadrants and multiply the robot
    counts in the quadrants.

    First Attempt:
    Loop through the robots for the desired number of seconds, add positions and velocities, making
    sure to wrap them.

    After the robots are done moving, count up the quadrants for the final score.

    Second Attempt:
    While the above works, we can do it faster. Instead of looping through all seconds, look ahead to
    the desired number of seconds, multiplying the velocity by the number of seconds, and keeping
    the result in bounds of the grid with modulo.

    NOTE:
    I updated the given input to include the height and area of the grid, since the project description
    gives us it but the input doesn't. Add a H=<height>,W=<width> to the top of any new input files.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int parserEndIdx;
    // NOTE: I modified the input to include the area height and width from the problem description.
    int height, width;
    lineLen = getline(&line, &lineCap, inputFile);
    height = parseNumber(line, 0, &parserEndIdx);
    width = parseNumber(line, parserEndIdx, &parserEndIdx);

    // The robot definitions, [pX, pY, vX, vY, ...]
    IntArray robots;
    initIntArray(&robots, INPUT_SIZE);
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        insertIntArray(&robots, parseNumber(line, 0, &parserEndIdx));
        insertIntArray(&robots, parseNumber(line, parserEndIdx, &parserEndIdx));
        insertIntArray(&robots, parseNumber(line, parserEndIdx, &parserEndIdx));
        insertIntArray(&robots, parseNumber(line, parserEndIdx, &parserEndIdx));
    }

    fclose(inputFile);

    // Move the robots for the set number of SECONDS.
    int pX, pY, vX, vY;
    // Keep track of where the robots end up.
    int quadNW = 0, quadNE = 0, quadSE = 0, quadSW = 0;
    // Move each robot.
    for (int idx = 0; idx < robots.numItems; idx += 4) {
        pX = robots.data[idx];
        pY = robots.data[idx + 1];
        vX = robots.data[idx + 2];
        vY = robots.data[idx + 3];

        // Move to the next position, accounting for warping to the other side.
        pX = (pX + (SECONDS * vX)) % width;
        // If our position is negative, add the grid width to bring us back in bounds.
        if (pX < 0) pX += width;

        pY = (pY + (SECONDS * vY)) % height;
        // If our position is negative, add the grid height to bring us back in bounds.
        if (pY < 0) pY += height;

        // Now, figure out what quadrant the robot lies in.

        // NW
        if (pY >= 0 && pY < height / 2 && pX >= 0 && pX < width / 2) quadNW += 1;
        
        // NE
        if (pY >= 0 && pY < height / 2 && pX > width / 2 && pX < width) quadNE += 1;

        // SW
        if (pY > height / 2 && pY < height && pX >= 0 && pX < width / 2) quadSW += 1;

        // SE
        if (pY > height / 2 && pY < height && pX > width / 2 && pX < width) quadSE += 1;
    }

    int safetyFactor = quadNE * quadNW * quadSE * quadSW;

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", safetyFactor, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void tryPrintTree(IntArray* robots, int height, int width, int secondsPassed) {
    /*
    Tries to figure out if the given robots make out a tree, and print their locations
    if it seems like they do.

    The used heuristic is 5 or more consecutive robots in a row, which works for my
    input (with some false-positives).

    If there are less than 5 consecutive robots in a row, I don't print the grid.
    */

    // Store the robot counts in a 2D grid.
    int map[height][width];
    memset(map, 0, width * height * sizeof(int));
    for (int idx = 0; idx < robots->numItems; idx += 4) {
        map[robots->data[idx + 1]][robots->data[idx]] += 1;
    }

    // Get the max number of consecutive robots in a row.
    int maxConsecutiveInARow = 0, rowCount = 0;
    for (int r = 0; r < height; r += 1) {
        rowCount = 0;
        for (int c = 0; c < width; c += 1) {
            // Reset if there's no robot in the spot we're looking at.
            if (map[r][c] == 0)
                rowCount = 0;
            else
                rowCount += 1;

            // Store the max.
            if (rowCount > maxConsecutiveInARow) maxConsecutiveInARow = rowCount;
        }
    }

    // If we've failed the heuristic, don't print it.
    if (maxConsecutiveInARow < 5) return;

    printf("Found potential tree after %ds\n", secondsPassed);
    for (int r = 0; r < height; r += 1) {
        for (int c = 0; c < width; c += 1) {
            if (map[r][c] == 0) {
                printf(".");
            } else {
                printf("%d", map[r][c]);
            }
        }
        printf("\n");
    }
}

void problem2(char* inputFilePath) {
    /*
    Given a certain number of seconds passed, the robots will make themselves into a nice picture
    of a tree! We need to find the number of seconds for the picture to show up.

    No description of what the tree would look like was given (I suspect to trip up LLMs), so I
    used a simple heuristic to check for in case of a likely tree: a certain number of consecutive
    bots in a row, which I guessed would make up the tree bottom, like:

        ...1.....1...
        ..111111111..

    I printed out the candidate trees and the seconds they appear at, and use my eyes to get
    the right answer.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int parserEndIdx;
    // NOTE: I modified the input to include the area height and width from the problem description.
    int height, width;
    lineLen = getline(&line, &lineCap, inputFile);
    height = parseNumber(line, 0, &parserEndIdx);
    width = parseNumber(line, parserEndIdx, &parserEndIdx);

    // The robot definitions, [pX, pY, vX, vY, ...]
    IntArray robots;
    initIntArray(&robots, INPUT_SIZE);
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        insertIntArray(&robots, parseNumber(line, 0, &parserEndIdx));
        insertIntArray(&robots, parseNumber(line, parserEndIdx, &parserEndIdx));
        insertIntArray(&robots, parseNumber(line, parserEndIdx, &parserEndIdx));
        insertIntArray(&robots, parseNumber(line, parserEndIdx, &parserEndIdx));
    }

    fclose(inputFile);

    // Move the robots for a large number of seconds, printing when we think we've
    // hit a tree shape, based on a heuristic.
    int SECONDS_MAX = 10000;
    int secondsPassed = 0;
    int pX, pY, vX, vY;
    while (secondsPassed < SECONDS_MAX) {
        // Move each robot.
        for (int idx = 0; idx < robots.numItems; idx += 4) {
            pX = robots.data[idx];
            pY = robots.data[idx + 1];
            vX = robots.data[idx + 2];
            vY = robots.data[idx + 3];

            pX = pX + vX;
            if (pX >= width)
                pX = pX - width;
            else if (pX < 0)
                pX = width + pX;

            pY = pY + vY;
            if (pY >= height)
                pY = pY - height;
            else if (pY < 0)
                pY = height + pY;

            robots.data[idx] = pX;
            robots.data[idx + 1] = pY;
        }

        secondsPassed += 1;

        // Attempt to print the tree, if it looks like there may be one.
        tryPrintTree(&robots, height, width, secondsPassed);
    }

    clock_t end = clock();
    printf("Problem 02 [%.2fms]\n", (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
