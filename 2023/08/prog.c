#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "../../utils/array.c"
#include "../../utils/map.c"
#include "../../utils/string.c"

int LEFT = 0;
int RIGHT = 1;


// Using Euclid's algorithm
long long gcd(long long a, long long b) {
    long long largerNumber = a > b ? a : b;
    long long smallerNumber = a > b ? b : a;

    while (largerNumber != smallerNumber) {
        long long difference = largerNumber - smallerNumber;

        if (difference > smallerNumber) {
            largerNumber = difference;
        } else {
            largerNumber = smallerNumber;
            smallerNumber = difference;
        }
    }

    return largerNumber;
}

long long lcm(long long a, long long b) {
    return (a * b) / gcd(a, b);
}


void problem1() {
    clock_t start = clock();

    FILE* inputFile = fopen("2023/08/input.txt", "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLength;

    int totalSteps = 0;

    // The directions to take in the maze, 0 = Left, 1 = Right.
    IntArray directions;
    initIntArray(&directions, 512);

    // Parse the directions.
    lineLength = getline(&line, &lineCap, inputFile);
    for (int idx = 0; idx < lineLength - 1; idx += 1) {
        insertIntArray(&directions, line[idx] == 'L' ? 0 : 1);
    }

    Map leftMaps, rightMaps;
    initMap(&leftMaps);
    initMap(&rightMaps);

    // Discard the empty line in the input file.
    getline(&line, &lineCap, inputFile);

    // Parse the graph into a map.
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        char node[4], left[4], right[4];

        // Format is: XXX = (YYY, ZZZ)
        substring(line, node, 0, 3);
        substring(line, left, 7, 10);
        substring(line, right, 12, 15);

        char* newNode = malloc(4 * sizeof(char));
        char* newLeft = malloc(4 * sizeof(char));
        char* newRight = malloc(4 * sizeof(char));
        strcpy(newNode, node);
        strcpy(newLeft, left);
        strcpy(newRight, right);

        setMap(&leftMaps, newNode, newLeft);
        setMap(&rightMaps, newNode, newRight);
    }

    // Traverse the graph until you hit node 'ZZZ'.
    char* currentNode = "AAA";
    int direction, directionIdx = 0;
    while (strcmp(currentNode, "ZZZ") != 0) {
        direction = directions.data[directionIdx];

        if (direction == LEFT) {
            getMap(&leftMaps, currentNode, &currentNode);
        } else {
            getMap(&rightMaps, currentNode, &currentNode);
        }

        totalSteps += 1;
        directionIdx = (directionIdx + 1) % directions.numItems;
    }

    freeMap(&leftMaps);
    freeMap(&rightMaps);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", totalSteps, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}


void problem2() {
    clock_t start = clock();

    FILE* inputFile = fopen("2023/08/input.txt", "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLength;

    // The directions to take in the maze, 0 = Left, 1 = Right.
    IntArray directions;
    initIntArray(&directions, 512);

    StringArray startingPoints;
    initStringArray(&startingPoints, 6, 4);

    // Parse the directions.
    lineLength = getline(&line, &lineCap, inputFile);
    for (int idx = 0; idx < lineLength - 1; idx += 1) {
        insertIntArray(&directions, line[idx] == 'L' ? 0 : 1);
    }

    Map leftMaps, rightMaps;
    initMap(&leftMaps);
    initMap(&rightMaps);

    // Discard the empty line in the input file.
    getline(&line, &lineCap, inputFile);

    // Parse the graph into a map.
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        char node[4], left[4], right[4];

        // Format is: XXX = (YYY, ZZZ)
        substring(line, node, 0, 3);
        substring(line, left, 7, 10);
        substring(line, right, 12, 15);

        char* newNode = malloc(4 * sizeof(char));
        char* newLeft = malloc(4 * sizeof(char));
        char* newRight = malloc(4 * sizeof(char));
        strcpy(newNode, node);
        strcpy(newLeft, left);
        strcpy(newRight, right);

        if (newNode[2] == 'A') {
            insertStringArray(&startingPoints, newNode);
        }

        setMap(&leftMaps, newNode, newLeft);
        setMap(&rightMaps, newNode, newRight);
    }

    // Traverse the graph until you hit node 'ZZZ'.
    char* currentNode = "AAA";
    int direction, directionIdx = 0;

    /**
     * Even though the problem statement doesn't guarantee it, LCM works for this and apparently
     * everyone elses input, which is kinda dumb. Basically all paths from the starts loop once
     * they hit the ending Z's, and the loops from there are the same length as the starting
     * paths. So we collect the lengths of each path and get the LCM which will get us the
     * step in which they all sync up.
    */
    int numPaths = startingPoints.numItems;

    int ghostSteps[numPaths];
    for (int idx = 0; idx < numPaths; idx += 1) {
        ghostSteps[idx] = 0;
    }

    bool allZ = false;
    while (!allZ) {
        direction = directions.data[directionIdx];

        for (int idx = 0; idx < numPaths; idx += 1) {
            if (startingPoints.data[idx][2] != 'Z') {
                ghostSteps[idx] += 1;

                if (direction == LEFT) {
                    getMap(&leftMaps, startingPoints.data[idx], &currentNode);
                } else {
                    getMap(&rightMaps, startingPoints.data[idx], &currentNode);
                }

                insertAtStringArray(&startingPoints, currentNode, idx);
            }
        }

        directionIdx = (directionIdx + 1) % directions.numItems;

        allZ = true;
        for (int idx = 0; idx < numPaths; idx += 1) {
            allZ = allZ && startingPoints.data[idx][2] == 'Z';
        }
    }

    // Calculate the LCM. LCM is commutative, so LCM(a, b, c) = LCM(LCM(a, b), c).
    long long totalStepsLCM = ghostSteps[0];
    for (int idx = 1; idx < numPaths; idx += 1) {
        totalStepsLCM = lcm(totalStepsLCM, ghostSteps[idx]);
    }

    freeMap(&leftMaps);
    freeMap(&rightMaps);
    freeStringArray(&startingPoints);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", totalStepsLCM, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}


int main() {
    problem1();
    problem2();

    return 0;
}