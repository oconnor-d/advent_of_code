#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

bool isSafe(IntArray* report) {
    /*
    Checks that the given report is safe. A report is safe if all levels are in increasing or
    decreasing order, and the difference between two sequential level is between 1 and 3 (inclusive).

    We get the direction the levels are going when looking at the first two levels, and check that
    directin doesn't flip further down the line. And then it's just a matter of checking that there's
    no too big or too small leaps between two levels.
    */

    // -1 = descending, 0 = unset (or possibly level), 1 = ascending.
    int direction = 0;
    int previousLevel, level, levelDifference;

    previousLevel = report->data[0];

    // We've already used idx 0 for `previousLevel`, so start at 1.
    for (int idx = 1; idx < report->numItems; idx += 1) {
        level = report->data[idx];

        // Set the direction the first two levels are heading.
        if (direction == 0) direction = level > previousLevel ? 1 : -1;

        // If we're ascending, level - previousLevel is positive, negative otherwise. So multiplying it by the known
        // direction ensures the `levelDifference` is positive IFF it's continuing in the same direction. If it's
        // switching direction, it'll be negative and out of bounds below.
        levelDifference = direction * (level - previousLevel);

        // Check if the difference is out of bounds, if so, the report is unsafe.
        if (levelDifference <= 0 || levelDifference >= 4) return false;

        previousLevel = level;
    }

    // We've found no bad levels.
    return true;
}

void problem1(char* inputFilePath) {
    /*
    The problem is asking us to check for valid reports, where all levels must either increase or decrease
    sequentially by intervals between 1 and 3 (inclusive).

    For this one, collect each report into an array and check that the number of errors in the report is
    0 to see if it's valid. See `getErrorCount` for the exact algorithm.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int safeReports = 0;
    int parserEndIdx = 0;

    IntArray report;
    initIntArray(&report, 10);
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        parserEndIdx = 0;
        // Reset the array without freeing up it's memory.
        report.numItems = 0;

        while (parserEndIdx < lineLen - 1) {
            insertIntArray(&report, parseNumber(line, parserEndIdx, &parserEndIdx));
        }

        if (isSafe(&report)) safeReports += 1;
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", safeReports, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    The problem this time is the same as in Problem 1, but with some wiggle room where a report can tolerate
    a single bad level.

    I tried to do this without brute-force but the edge cases were annoying me, so instead I first check
    if the whole report is valid (as it would be for Problem 1). If it is, great! If not, I check if
    subsets of the report (each without a level) are valid. So for a report of [1, 6, 4], it checks
    [1, 6, 4], and since that's invalid it then checks [1, 6], also invalid, until it gets to the
    valid [6, 4]. Since at least one sub-list with a single level removed it's valid, the entire
    report falls within the fault tolerance and counts as valid.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int safeReports = 0;
    int parserEndIdx = 0;
    int idxToExclude = 0;

    // Report holds the entire line, subReport will hold the entire line - a specific level.
    IntArray report, subReport;
    initIntArray(&report, 10);
    initIntArray(&subReport, 10);
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        parserEndIdx = 0;
        // Reset the arrays without freeing up their memory.
        report.numItems = 0;
        subReport.numItems = 0;

        while (parserEndIdx < lineLen - 1) {
            insertIntArray(&report, parseNumber(line, parserEndIdx, &parserEndIdx));
        }

        // First, check if the entire report is safe.
        if (isSafe(&report)) {
            safeReports += 1;
        } else {
            // If the whole report isn't safe, start checking all sub-permutations with one level removed.
            idxToExclude = 0;
            while (idxToExclude < report.numItems) {
                for (int idx = 0; idx < report.numItems; idx += 1) {
                    if (idx != idxToExclude) insertIntArray(&subReport, report.data[idx]);
                }

                if (isSafe(&subReport)) {
                    // Once we find a single report without any errors, there's no need to check the rest.
                    safeReports += 1;
                    break;
                }

                idxToExclude += 1;
                subReport.numItems = 0;
            }
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", safeReports, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
