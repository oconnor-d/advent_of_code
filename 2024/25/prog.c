#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_INPUTS 600
#define PINS 5
#define LOCK_HEIGHT 5

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // The pin heights of the input locks and keys.
    int keys[MAX_INPUTS][PINS] = {0};
    int locks[MAX_INPUTS][PINS] = {0};

    // Count the lock/key pin heights.
    bool isKey;
    int keyIdx = 0, lockIdx = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        isKey = line[0] == '.';

        for (int r = 0; r < LOCK_HEIGHT; r += 1) {
            // Tally up the filled space in each pin at each height level.
            getline(&line, &lineCap, inputFile);
            for (int c = 0; c < PINS; c += 1) {
                if (line[c] == '#') {
                    if (isKey)
                        keys[keyIdx][c] += 1;
                    else
                        locks[lockIdx][c] += 1;
                }
            }
        }

        if (isKey)
            keyIdx += 1;
        else
            lockIdx += 1;

        // Skip the last row and the empty space between keys/locks, breaking once the EOF is hit.
        if ((lineLen = getline(&line, &lineCap, inputFile)) <= 0) break;
        if ((lineLen = getline(&line, &lineCap, inputFile)) <= 0) break;
    }
    fclose(inputFile);

    int numKeys = keyIdx, numLocks = lockIdx;

    // Try each key on every lock.
    bool keyFits;
    int fitCount = 0;
    for (keyIdx = 0; keyIdx < numKeys; keyIdx += 1) {
        for (lockIdx = 0; lockIdx < numLocks; lockIdx += 1) {
            // A key fits if all it's pins fit, and a pin fits if it's height plus the lock's
            // height doesn't exceed the available space in a lock (i.e. LOCK_HEIGHT).
            keyFits = true;
            for (int idx = 0; idx < PINS; idx += 1) {
                if (keys[keyIdx][idx] + locks[lockIdx][idx] > LOCK_HEIGHT) {
                    keyFits = false;
                    break;
                }
            }

            if (keyFits) fitCount += 1;
        }
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", fitCount, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);

    return 0;
}
