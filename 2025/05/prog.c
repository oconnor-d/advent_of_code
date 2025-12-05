#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

#define MAX_RANGES 250

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // 1. Collect the ranges as a list of [start1, end1, start2, end2, ...]
    LLongArray validRanges;
    initLLongArray(&validRanges, 100);

    int endIdx;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        if (line[0] == '\n') break;

        insertLLongArray(&validRanges, parseNumber(line, 0, &endIdx));
        insertLLongArray(&validRanges, parseNumber(line, endIdx + 1, &endIdx));
    }

    // 2. Count the IDs within any of the ranges.
    long long numValid = 0;
    long long num;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        num = parseNumber(line, 0, &endIdx);
        for (int idx = 0; idx < validRanges.numItems; idx += 2) {
            if (num >= validRanges.data[idx] && num <= validRanges.data[idx + 1]) {
                numValid += 1;
                break;
            }
        }
    }
    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", numValid, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // 1. Get the list of ranges.
    LLongArray ranges;
    initLLongArray(&ranges, 100);

    int endIdx;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        if (line[0] == '\n') break;

        insertLLongArray(&ranges, parseNumber(line, 0, &endIdx));
        insertLLongArray(&ranges, parseNumber(line, endIdx + 1, &endIdx));
    }

    // 2. Continuously merge overlapping ranges until no more can be merged.
    long long rangeStart, rangeEnd;
    bool mergedRange = true;
    while (mergedRange) {
        for (int currentIdx = 0; currentIdx < ranges.numItems; currentIdx += 2) {
            rangeStart = ranges.data[currentIdx];
            rangeEnd = ranges.data[currentIdx + 1];
            mergedRange = false;

            for (int idx = 0; idx < ranges.numItems; idx += 2) {
                // Don't try and merge the range with itself.
                if (idx == currentIdx) continue;
                // When a range gets merged into another one, the range gets "removed" by setting
                // both its values to -1. So, ignore invalid ranges.
                if (rangeStart == -1 || ranges.data[idx] == -1) continue;

                // Second range contains current one, remove current.
                if (ranges.data[idx] <= rangeStart && ranges.data[idx + 1] >= rangeEnd) {
                    mergedRange = true;
                }
                // Current range contains second one, replace second one with current.
                else if (rangeStart <= ranges.data[idx] && rangeEnd >= ranges.data[idx + 1]) {
                    mergedRange = true;

                    ranges.data[idx] = rangeStart;
                    ranges.data[idx + 1] = rangeEnd;
                }
                // Second range contains current start, extend second range end to current range end.
                else if (ranges.data[idx] <= rangeStart && ranges.data[idx + 1] >= rangeStart) {
                    ranges.data[idx + 1] = rangeEnd;
                    mergedRange = true;
                }
                // Second range contains current end, extend second range start to current range start.
                else if (ranges.data[idx] <= rangeEnd && ranges.data[idx + 1] >= rangeEnd) {
                    ranges.data[idx] = rangeStart;
                    mergedRange = true;
                }

                // If the range was merged, "remove" it.
                if (mergedRange) {
                    ranges.data[currentIdx] = -1;
                    ranges.data[currentIdx + 1] = -1;
                }
            }
        }
    }

    // 3. Count up the non-overlapping ranges.
    long long numValid = 0;
    for (int idx = 0; idx < ranges.numItems; idx += 2) {
        // Ignore "removed" ranges.
        if (ranges.data[idx] == -1) continue;

        numValid += (ranges.data[idx + 1] - ranges.data[idx] + 1);
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", numValid, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
