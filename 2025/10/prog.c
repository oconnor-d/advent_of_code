#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

#define MAX_LIGHTS 100

#define OFF '.'
#define ON '#'

int toDecimal(bool* buffer, int size) {
    int number = 0;

    int power = 0;
    for (int idx = size - 1; idx >= 0; idx -= 1) {
        if (buffer[idx]) {
            number += pow(2, power);
        }
        power += 1;
    }

    return number;
}

bool nestLookup(IntArray* wirings, int depth, int current, int target) {
    if (current == target) return true;
    if (depth <= 0) return false;

    bool hi;
    for (int idx = 0; idx < wirings->numItems; idx += 1) {
        hi = nestLookup(wirings, depth - 1, current ^ wirings->data[idx], target);
        if (hi) return true;
    }

    return false;
}

void problem1(char* inputFilePath) {
    clock_t start = clock();

    bool lights[MAX_LIGHTS];
    bool targetLights[MAX_LIGHTS];
    bool wiring[MAX_LIGHTS];
    int buttonSize;
    IntArray wirings;
    initIntArray(&wirings, 10);

    int total = 0;

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 1) {
        wirings.numItems = 0;
        // Reset all lights
        for (int idx = 0; idx < MAX_LIGHTS; idx += 1) {
            lights[idx] = 0;
            targetLights[idx] = 0;
        }

        int idx = 0;
        while (line[idx + 1] != ']') {
            if (line[idx + 1] == ON) targetLights[idx] = true;
            idx += 1;
        }

        buttonSize = idx;
        idx += 3;

        while (line[idx] != '{') {
            for (int i = 0; i < buttonSize; i += 1) wiring[i] = false;

            while (line[idx] != ')') {
                idx += 1;
                wiring[line[idx] - '0'] = true;
                idx += 1;
            }

            insertIntArray(&wirings, toDecimal(wiring, buttonSize));
            idx += 2;
        }

        int target = toDecimal(targetLights, buttonSize);

        // Increase and get permutations until we have a valid solution.
        int numPresses = 1;
        bool found = false;
        while (!found) {
            for (int i = 0; i < wirings.numItems; i += 1) {
                if (nestLookup(&wirings, numPresses - 1, wirings.data[i], target)) {
                    found = true;
                    printf("%d\n", numPresses);
                    total += numPresses;
                    break;
                }
            }

            numPresses += 1;
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", total, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
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
