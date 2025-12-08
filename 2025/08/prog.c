#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/string.c"

#define MAX_JBOXES 1000
#define CONNECTIONS 190

// Each jbox is represented as (x, y, z, circuit_id), where
int grid[MAX_JBOXES][4];

long distance(int p1, int p2) {
    long d1 = grid[p1][0] - grid[p2][0];
    long d2 = grid[p1][1] - grid[p2][1];
    long d3 = grid[p1][2] - grid[p2][2];
    return sqrt((d1 * d1) + (d2 * d2) + (d3 * d3));
}

int comp(const void* a, const void* b) {
    return (*(int*)b - *(int*)a);
}

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    int endIdx, numJboxes = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        grid[numJboxes][0] = parseNumber(line, 0, &endIdx);
        grid[numJboxes][1] = parseNumber(line, endIdx, &endIdx);
        grid[numJboxes][2] = parseNumber(line, endIdx, &endIdx);
        grid[numJboxes][3] = numJboxes;  // no circuit
        numJboxes += 1;
    }

    fclose(inputFile);

    // TO FIGURE OUT:
    // 1. Can two circuits merge into one big circuit, this code is assuming no.
    // 2. Need better visualization.

    bool isConnected[MAX_JBOXES][MAX_JBOXES] = {false};
    for (int idx = 0; idx < numJboxes; idx += 1) isConnected[idx][idx] = true;

    int shortestIdx1, shortestIdx2;
    long shortestDistance;
    for (int connection = 0; connection < CONNECTIONS; connection += 1) {
        shortestDistance = INT_MAX;

        for (int idx = 0; idx < numJboxes; idx += 1) {
            for (int otherIdx = 0; otherIdx < numJboxes; otherIdx += 1) {
                if (!isConnected[idx][otherIdx] && distance(idx, otherIdx) < shortestDistance) {
                    shortestDistance = distance(idx, otherIdx);
                    shortestIdx1 = idx;
                    shortestIdx2 = otherIdx;
                }
            }
        }

        isConnected[shortestIdx1][shortestIdx2] = true;
        isConnected[shortestIdx2][shortestIdx1] = true;

        int circuitToMerge = grid[shortestIdx2][3];
        for (int i = 0; i < numJboxes; i += 1) {
            if (grid[i][3] == circuitToMerge) grid[i][3] = grid[shortestIdx1][3];
        }
    }

    for (int idx = 0; idx < numJboxes; idx += 1) printf("%d\n", grid[idx][3]);

    int circuitSizes[MAX_JBOXES] = {0};
    for (int idx = 0; idx < numJboxes; idx += 1) circuitSizes[grid[idx][3]] += 1;

    qsort(circuitSizes, numJboxes, sizeof(int), comp);
    for (int idx = 0; idx < numJboxes; idx += 1) {
        printf("%d ", circuitSizes[idx]);
    }
    printf("\n");
    long long total = circuitSizes[0] * circuitSizes[1] * circuitSizes[2];

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", total, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    int endIdx, numJboxes = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        grid[numJboxes][0] = parseNumber(line, 0, &endIdx);
        grid[numJboxes][1] = parseNumber(line, endIdx, &endIdx);
        grid[numJboxes][2] = parseNumber(line, endIdx, &endIdx);
        grid[numJboxes][3] = numJboxes;  // no circuit
        numJboxes += 1;
    }

    fclose(inputFile);

    // TO FIGURE OUT:
    // 1. Can two circuits merge into one big circuit, this code is assuming no.
    // 2. Need better visualization.

    bool isConnected[MAX_JBOXES][MAX_JBOXES] = {false};
    for (int idx = 0; idx < numJboxes; idx += 1) isConnected[idx][idx] = true;

    int shortestIdx1, shortestIdx2;
    long shortestDistance;
    int circuitSizes[MAX_JBOXES];
    for (int i = 0; i < numJboxes; i += 1) circuitSizes[i] = 1;

    while (true) {
        shortestDistance = INT_MAX;

        for (int idx = 0; idx < numJboxes; idx += 1) {
            for (int otherIdx = 0; otherIdx < numJboxes; otherIdx += 1) {
                if (!isConnected[idx][otherIdx] && distance(idx, otherIdx) < shortestDistance) {
                    shortestDistance = distance(idx, otherIdx);
                    shortestIdx1 = idx;
                    shortestIdx2 = otherIdx;
                }
            }
        }

        isConnected[shortestIdx1][shortestIdx2] = true;
        isConnected[shortestIdx2][shortestIdx1] = true;

        if (grid[shortestIdx1][3] != grid[shortestIdx2][3]) {
            circuitSizes[shortestIdx1] += circuitSizes[shortestIdx2];

            int circuitToMerge = grid[shortestIdx2][3];
            for (int i = 0; i < numJboxes; i += 1) {
                if (i != shortestIdx1 && grid[i][3] == circuitToMerge) {
                    grid[i][3] = grid[shortestIdx1][3];
                }

                if (grid[i][3] == grid[shortestIdx1][3]) circuitSizes[i] = circuitSizes[shortestIdx1];
            }

            // for (int idx = 0; idx < numJboxes; idx += 1) printf("%d ", circuitSizes[idx]);
            // printf("\n");

            if (circuitSizes[shortestIdx1] == 1000) {
                printf("[%d] %d <-> %d = %lld\n", circuitSizes[shortestIdx1], grid[shortestIdx1][0], grid[shortestIdx2][0], (long long)grid[shortestIdx1][0] * (long long)grid[shortestIdx2][0]);
                return;
            }
        }
    }

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", 0, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    // problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
