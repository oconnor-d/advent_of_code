#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/set.c"

#define ASTEROID '#'

void problem1(char* inputFilePath) {
    /*
    In a grid of asteroids, get the max number of asteroids visible from another asteroid.

    Only one asteroid is visible on any line along an angle from a starting asteroid, so the
    number of visible asteroids from an asteroid is the number of unique angles from an asteroid
    to all other asteroids.

    This calls for a set, but I don't have a double set and don't feel like creating one yet, so
    I store the seen angles in a list which is why this solution is much slower than it should be.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Get all coords of the asteroids.
    IntArray asteroidCoords;
    initIntArray(&asteroidCoords, 256);

    int x = 0, y = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        for (x = 0; x < lineLen; x += 1) {
            if (line[x] == ASTEROID) {
                insertIntArray(&asteroidCoords, x);
                insertIntArray(&asteroidCoords, y);
            }
        }
        y += 1;
    }
    fclose(inputFile);

    // For each asteroid, keep track of unique angles to other asteroids.
    int seenCount;
    double seenAngles[asteroidCoords.numItems];

    int maxVisibility = INT_MIN;
    int bestX, bestY;
    int baseX, baseY, destX, destY;
    double angle;
    bool alreadySeen;
    for (int idx = 0; idx < asteroidCoords.numItems; idx += 2) {
        baseX = asteroidCoords.data[idx];
        baseY = asteroidCoords.data[idx + 1];

        // Reset the seen asteroid "set"
        seenCount = 0;
        for (int sIdx = 0; sIdx < asteroidCoords.numItems; sIdx += 1) seenAngles[sIdx] = -1;

        for (int destIdx = 0; destIdx < asteroidCoords.numItems; destIdx += 2) {
            destX = asteroidCoords.data[destIdx];
            destY = asteroidCoords.data[destIdx + 1];

            // Don't count an asteroid as visible to itself.
            if (baseX == destX && baseY == destY) continue;

            angle = atan2(destY - baseY, destX - baseX) * 180 / M_PI;

            // Check that the angle is unique.
            alreadySeen = false;
            for (int sIdx = 0; sIdx < asteroidCoords.numItems; sIdx += 1) {
                if (seenAngles[sIdx] == angle) {
                    alreadySeen = true;
                    break;
                }
            }

            if (!alreadySeen) {
                seenAngles[seenCount] = angle;
                seenCount += 1;
            }
        }

        if (seenCount > maxVisibility) {
            bestX = baseX;
            bestY = baseY;
            maxVisibility = seenCount;
        }
    }

    clock_t end = clock();
    printf("Problem 01: (%d, %d) %d [%.2fms]\n", bestX, bestY, maxVisibility, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Skipping for now, I'm mostly interested in the Intcode problems.

    Generally, we know the angles, so just sort the closer asteroids on the angle (manhattan distance) before
    further asteroids on the same angle, and only destroy one asteroid per angle (going clockwise) at a time.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", 0, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
