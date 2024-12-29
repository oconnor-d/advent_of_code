#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

void loadCoordinatesAndSteps(char* line, ssize_t lineLen, IntArray* output) {
    /*
    Loads the entire wire path into the output array, alongside the steps taken
    at each coordinate, like:

        [x1, y1, 0, x2, y2, 1, ...]
    */
    initIntArray(output, 1024);
    insertIntArray(output, 0);
    insertIntArray(output, 0);
    insertIntArray(output, 0);

    int steps = 0;

    int parserEndIdx = 0;
    int prevX, prevY;
    char direction;
    int moveDistance;
    while (parserEndIdx < lineLen - 1) {
        prevX = output->data[output->numItems - 3];
        prevY = output->data[output->numItems - 2];

        direction = line[parserEndIdx];

        moveDistance = parseNumber(line, parserEndIdx, &parserEndIdx);
        // Move past the "," in the input, pointing to the next direction.
        parserEndIdx += 1;

        for (int step = 1; step <= moveDistance; step += 1) {
            steps += 1;

            if (direction == 'R') {
                insertIntArray(output, prevX + step);
                insertIntArray(output, prevY);
            } else if (direction == 'L') {
                insertIntArray(output, prevX - step);
                insertIntArray(output, prevY);
            } else if (direction == 'U') {
                insertIntArray(output, prevX);
                insertIntArray(output, prevY - step);
            } else if (direction == 'D') {
                insertIntArray(output, prevX);
                insertIntArray(output, prevY + step);
            }

            insertIntArray(output, steps);
        }
    }
}

void problem1(char* inputFilePath) {
    /*
    The problem's asking for the closest intersection of two wires.

    This solution is horribly slow - I load ALL coordinates the wire occupies into a list,
    and then check against ALL coordinates the other wire does to check for intersections.

    This could be immediately improved by storing the locations in a set for quick lookup,
    or by using some geometry probably.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // [x1, y1, 0, x2, y2, 1, ...]
    IntArray wire1Coords;
    lineLen = getline(&line, &lineCap, inputFile);
    loadCoordinatesAndSteps(line, lineLen, &wire1Coords);

    // [x1, y1, 0, x2, y2, 1, ...]
    IntArray wire2Coords;
    lineLen = getline(&line, &lineCap, inputFile);
    loadCoordinatesAndSteps(line, lineLen, &wire2Coords);

    // Check for any crossing lines, keeping track of the minimum distance one.
    int minIntersectionDistance = INT_MAX, intersectionDistance;
    int x1, y1, x2, y2;
    int maxX, maxY;
    // Ignore steps for this part.
    for (int wire1Idx = 3; wire1Idx < wire1Coords.numItems; wire1Idx += 3) {
        x1 = wire1Coords.data[wire1Idx];
        y1 = wire1Coords.data[wire1Idx + 1];

        for (int wire2Idx = 3; wire2Idx < wire2Coords.numItems; wire2Idx += 3) {
            x2 = wire2Coords.data[wire2Idx];
            y2 = wire2Coords.data[wire2Idx + 1];

            if (x1 == x2 && y1 == y2) {
                intersectionDistance = abs(x1) + abs(y1);
                minIntersectionDistance = intersectionDistance < minIntersectionDistance ? intersectionDistance : minIntersectionDistance;
            }
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", minIntersectionDistance, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    The problem's asking for the least amount of steps to any intersection of two wires.

    This solution is horribly slow as well, and can be improved the same way as part 1.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // [x1, y1, 0, x2, y2, 1, ...]
    IntArray wire1Coords;
    lineLen = getline(&line, &lineCap, inputFile);
    loadCoordinatesAndSteps(line, lineLen, &wire1Coords);

    // [x1, y1, 0, x2, y2, 1, ...]
    IntArray wire2Coords;
    lineLen = getline(&line, &lineCap, inputFile);
    loadCoordinatesAndSteps(line, lineLen, &wire2Coords);

    // Check for any crossing lines, keeping track of the minimum distance one.
    int minIntersectionSteps = INT_MAX, intersectionSteps;
    int x1, y1, steps1, x2, y2, steps2;
    for (int wire1Idx = 3; wire1Idx < wire1Coords.numItems; wire1Idx += 3) {
        x1 = wire1Coords.data[wire1Idx];
        y1 = wire1Coords.data[wire1Idx + 1];
        steps1 = wire1Coords.data[wire1Idx + 2];

        for (int wire2Idx = 3; wire2Idx < wire2Coords.numItems; wire2Idx += 3) {
            x2 = wire2Coords.data[wire2Idx];
            y2 = wire2Coords.data[wire2Idx + 1];
            steps2 = wire2Coords.data[wire2Idx + 2];

            if (x1 == x2 && y1 == y2) {
                intersectionSteps = steps1 + steps2;
                minIntersectionSteps = intersectionSteps < minIntersectionSteps ? intersectionSteps : minIntersectionSteps;
            }
        }
    }

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", minIntersectionSteps, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
