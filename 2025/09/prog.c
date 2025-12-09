#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

#define MAX_COORDS 500

void problem1(char* inputFilePath) {
    clock_t start = clock();

    // [x1, y1, x2, y2, ...]
    int coords[MAX_COORDS * 2];
    int numCoords = 0;

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    int endIdx;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        coords[numCoords] = parseNumber(line, 0, &endIdx);
        coords[numCoords + 1] = parseNumber(line, endIdx + 1, &endIdx);

        numCoords += 2;
    }

    fclose(inputFile);

    long area, maxArea = INT_MIN;
    for (int i = 0; i < numCoords; i += 2) {
        for (int j = i + 2; j < numCoords; j += 2) {
            area = (long)(abs(coords[i] - coords[j]) + 1) * (abs(coords[i + 1] - coords[j + 1]) + 1);
            if (area > maxArea) maxArea = area;
        }
    }

    clock_t end = clock();
    printf("Problem 01: %ld [%.2fms]\n", maxArea, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    // [x1, y1, x2, y2, ...]
    int coords[MAX_COORDS * 2];
    int numCoords = 0;

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    int endIdx;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        coords[numCoords] = parseNumber(line, 0, &endIdx);
        coords[numCoords + 1] = parseNumber(line, endIdx + 1, &endIdx);

        numCoords += 2;
    }

    fclose(inputFile);

    // Get a list of all coords around the perimeter of the polygon. We'll check that the rectangle doesn't cross
    // any of the points. This is slow but I'm bad at geometry and don't want to figure out a faster check.
    IntArray perimeterCoords;
    initIntArray(&perimeterCoords, 1500000);
    int x1, y1, x2, y2;
    int minX, minY, maxX, maxY;
    for (int idx = 0; idx < numCoords; idx += 2) {
        x1 = coords[idx];
        y1 = coords[idx + 1];
        // Wrap around at the end.
        if (idx == numCoords - 2) {
            x2 = coords[0];
            y2 = coords[1];
        } else {
            x2 = coords[idx + 2];
            y2 = coords[idx + 3];
        }

        minX = x1 < x2 ? x1 : x2;
        minY = y1 < y2 ? y1 : y2;
        maxX = x1 < x2 ? x2 : x1;
        maxY = y1 < y2 ? y2 : y1;

        if (x1 == x2) {
            for (int y = minY; y <= maxY; y += 1) {
                insertIntArray(&perimeterCoords, x1);
                insertIntArray(&perimeterCoords, y);
            }
        } else {
            for (int x = minX; x <= maxX; x += 1) {
                insertIntArray(&perimeterCoords, x);
                insertIntArray(&perimeterCoords, y1);
            }
        }
    }

    long area,
        maxArea = INT_MIN;
    int px, py;
    bool isInside;
    for (int i = 0; i < numCoords; i += 2) {
        x1 = coords[i];
        y1 = coords[i + 1];

        for (int j = i + 2; j < numCoords; j += 2) {
            x2 = coords[j];
            y2 = coords[j + 1];

            // Only figure out if we're inside the rectangle if we've found a bigger area.
            area = (long)(abs(x1 - x2) + 1) * (abs(y1 - y2) + 1);
            if (area <= maxArea) continue;

            minX = x1 < x2 ? x1 : x2;
            minY = y1 < y2 ? y1 : y2;
            maxX = x1 < x2 ? x2 : x1;
            maxY = y1 < y2 ? y2 : y1;

            isInside = true;
            for (int p = 0; p < perimeterCoords.numItems; p += 2) {
                px = perimeterCoords.data[p];
                py = perimeterCoords.data[p + 1];

                /*
                Check that the rectangle doesn't cross the perimeter, like this:

                    (minX, minY)
                        \
                      (px, py)
                          \
                      (maxX, maxY)
                */
                if (minX < px && px < maxX && minY < py && py < maxY) {
                    isInside = false;
                    break;
                }
            }

            if (isInside) {
                maxArea = area;
            }
        }
    }

    clock_t end = clock();
    printf("Problem 02: %ld [%.2fms]\n", maxArea, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
