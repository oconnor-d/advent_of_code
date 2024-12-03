#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/string.c"

int cmp_int(const void* va, const void* vb) {
    int a = *(int*)va, b = *(int*)vb;

    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

void problem1(char* inputFilePath) {
    // The problem is asking us to calculate the differences of the smallest pairs in both lists, so we'll
    // just sort both of them from low to high and sum up the difference sequentially.

    clock_t start = clock();

    int leftLocations[1000], rightLocations[1000];

    int locationIdx = 0;
    int numberStartIdx, numberEndIdx;
    FILE* inputFile = fopen(inputFilePath, "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        leftLocations[locationIdx] = parseFirstNumber(line, &numberStartIdx, &numberEndIdx);
        rightLocations[locationIdx] = parseFirstNumber(line + numberEndIdx, &numberStartIdx, &numberEndIdx);

        locationIdx += 1;
    }

    fclose(inputFile);

    // Sort both arrays from low to high.
    qsort(leftLocations, locationIdx, sizeof(int), cmp_int);
    qsort(rightLocations, locationIdx, sizeof(int), cmp_int);

    // Sum up the differences.
    int totalDifference = 0;
    for (int idx = 0; idx < locationIdx; idx += 1) {
        totalDifference += abs(leftLocations[idx] - rightLocations[idx]);
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", totalDifference, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    // Instead of getting the difference of the smallest pairs, we're multiplying each number in the left list
    // by the number of times it appears in the right list.
    //
    // If both lists are sorted as in Problem 1, we can do this in a single pass over both lists and without using a
    // hash map (which I haven't implemented yet) to store the counts.
    //
    // The algorithm goes like this, iterating over the left list:
    //
    // Fast forward where we are in the right list until we find a value that matches or exceeds the number in the left list. We know
    // any number in the right list that's less than the one we're currently at in the left list isn't present in the left list at all,
    // otherwise we would've processed itm on a previous iteration.
    //
    // Then, if the numbers are the same, keep traversing the right list until they're no longer the same, keeping track of the total
    // count of matching numbers in the right list. Once we've done that we have the count to multiply with and we can move on
    // to the next number in the left list.
    //
    // If the number in the right list is larger than the one in the left, the left number isn't present in the right list at
    // all and we can move on to the next number in the left.
    //
    // To handle multiple of the same number in the last list, we cache the number count of the last iteration and just use
    // that for duplicate numbers, this way we don't have to recount the right list.

    clock_t start = clock();

    int leftLocations[1000], rightLocations[1000];

    int locationIdx = 0;
    int numberStartIdx, numberEndIdx;
    FILE* inputFile = fopen(inputFilePath, "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        leftLocations[locationIdx] = parseFirstNumber(line, &numberStartIdx, &numberEndIdx);
        rightLocations[locationIdx] = parseFirstNumber(line + numberEndIdx, &numberStartIdx, &numberEndIdx);

        locationIdx += 1;
    }

    fclose(inputFile);

    // Sort both arrays from low to high.
    qsort(leftLocations, locationIdx, sizeof(int), cmp_int);
    qsort(rightLocations, locationIdx, sizeof(int), cmp_int);

    // Sum the similarity score for each number in the left list.
    int similarityScore = 0;
    int lastNumCount = 0;
    int lastNum = -1;
    int rightIdx = 0;
    for (int leftIdx = 0; leftIdx < locationIdx; leftIdx += 1) {
        // We already counted this number, so no need to count again.
        if (leftLocations[leftIdx] == lastNum) {
            similarityScore += lastNum * lastNumCount;
            continue;
        }

        lastNum = leftLocations[leftIdx];
        lastNumCount = 0;

        // skip all numbers less than the left number we're at.
        while (rightLocations[rightIdx] < leftLocations[leftIdx]) {
            rightIdx += 1;
        }

        // we've hit our mark, get the total count of matching numbers in the right list.
        while (rightLocations[rightIdx] == leftLocations[leftIdx]) {
            lastNumCount += 1;
            rightIdx += 1;
        }

        similarityScore += lastNum * lastNumCount;
    }

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", similarityScore, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
