#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "../../utils/array.c"
#include "../../utils/string.c"


long long min(long a, long b) {
    return (a < b) ? a : b;
}


void problem1() {
    clock_t start = clock();
    
    FILE* inputFile = fopen("2023/05/input.txt", "r");

    char* line = NULL;
    char* buffer;
    size_t lineCap = 0;
    ssize_t lineLength;

    // First, load the seeds up into an array.

    LLongArray seeds;
    initLLongArray(&seeds, 20);

    getline(&line, &lineCap, inputFile);
    // the first seed starts 7 characters into
    buffer = line + 7;
    int seedStartIdx, seedEndIdx;
    while (*buffer) {
        insertLLongArray(&seeds, parseFirstNumber(buffer, &seedStartIdx, &seedEndIdx));
        buffer += seedEndIdx + 1;
    }

    bool hasBeenMapped[seeds.numItems];
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        if (*line == '\n') continue;
        if (isalpha(*line)) {
            for (int idx = 0; idx < seeds.numItems; idx += 1) {
                hasBeenMapped[idx] = false;
            }
            continue;
        }

        buffer = line;

        int startIdx, endIdx;
        long long destStart = parseFirstNumber(buffer, &startIdx, &endIdx);
        buffer += endIdx + 1;

        long long sourceStart = parseFirstNumber(buffer, &startIdx, &endIdx);
        buffer += endIdx + 1;

        long long range = parseFirstNumber(buffer, &startIdx, &endIdx);

        for (int idx = 0; idx < seeds.numItems; idx += 1) {
            if (seeds.data[idx] >= sourceStart && seeds.data[idx] < sourceStart + range && !hasBeenMapped[idx]) {
                seeds.data[idx] = destStart + (seeds.data[idx] - sourceStart);
                hasBeenMapped[idx] = true;
            }
        }
    }

    long long minSeed = seeds.data[0];
    for (int idx = 1; idx < seeds.numItems; idx += 1) {
        minSeed = min(minSeed, seeds.data[idx]);
    }

    freeLLongArray(&seeds);

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", minSeed, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}



void problem2() {
    clock_t start = clock();
    
    FILE* inputFile = fopen("2023/05/input.txt", "r");

    char* line = NULL;
    char* buffer;
    size_t lineCap = 0;
    ssize_t lineLength;

    // First, load the seeds up into an array.
    // The seeds are of the form [seedStart, seedRangeLength, seedRangeHasBeenMapped, ...], where seedRangeHasBeenMapped means
    // that the seeds represented by that range have already been transformed in the current x->y group mappings (0 = false, 1 = true).
    LLongArray seeds;
    initLLongArray(&seeds, 512);

    LLongArray seedRanges;
    initLLongArray(&seedRanges, 1028);

    getline(&line, &lineCap, inputFile);
    // the first seed starts 7 characters into
    buffer = line + 7;
    int seedStartIdx, seedEndIdx;
    while (*buffer) {
        insertLLongArray(&seeds, parseFirstNumber(buffer, &seedStartIdx, &seedEndIdx));
        buffer += seedEndIdx + 1;
        insertLLongArray(&seeds, parseFirstNumber(buffer, &seedStartIdx, &seedEndIdx));
        buffer += seedEndIdx + 1;
        insertLLongArray(&seeds, 0);
    }

    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        if (*line == '\n') continue;
        if (isalpha(*line)) {
            if (isalpha(*line)) {
                // Reset the mapping trackers in between x->y groups.
                for (int idx = 2; idx < seeds.numItems; idx += 3) {
                    seeds.data[idx] = 0;
                }
            }
            continue;
        }

        buffer = line;

        int startIdx, endIdx;
        long long destStart = parseFirstNumber(buffer, &startIdx, &endIdx);
        buffer += endIdx + 1;

        long long sourceStart = parseFirstNumber(buffer, &startIdx, &endIdx);
        buffer += endIdx + 1;

        long long sourceRange = parseFirstNumber(buffer, &startIdx, &endIdx);

        // Inclusive start, inclusive end.
        long long sourceEnd = sourceStart + sourceRange - 1;

        for (int idx = 0; idx < seeds.numItems; idx += 3) {
            // start and end inclusive
            long long seedStart = seeds.data[idx];
            long long seedRange = seeds.data[idx + 1];
            long long hasBeenMapped = seeds.data[idx + 2];
            long long seedEnd = seedStart + seedRange - 1;
            
            // Case 1: seed range entirely within mapping range.
            // Seed: [5, 10], Mapping: [100, 0, 20] -> [105, 10]
            if (seedStart >= sourceStart && seedEnd <= sourceEnd && !hasBeenMapped) {
                insertLLongArray(&seedRanges, destStart + (seedStart - sourceStart));
                insertLLongArray(&seedRanges, seedRange);
                insertLLongArray(&seedRanges, 1);
            // Case 2: mapping range entirely within seed range.
            // Seed: [0, 20], Mapping: [100, 5, 10] -> [0, 5], [100, 10], [15, 5]
            } else if (sourceStart >= seedStart && sourceEnd <= seedEnd && !hasBeenMapped) {
                long long firstRange = sourceStart - seedStart;
                insertLLongArray(&seedRanges, seedStart);
                insertLLongArray(&seedRanges, firstRange);
                insertLLongArray(&seedRanges, hasBeenMapped);

                insertLLongArray(&seedRanges, destStart);
                insertLLongArray(&seedRanges, sourceRange);
                insertLLongArray(&seedRanges, 1);

                insertLLongArray(&seedRanges, seedStart + sourceRange + (sourceStart - seedStart));
                insertLLongArray(&seedRanges, seedRange - sourceRange - firstRange);
                insertLLongArray(&seedRanges, hasBeenMapped);
            // Case 3: seed range starts before mapping range but ends within it.
            // Seed: [0, 20], Mapping: [100, 10, 20] -> [0, 10], [100, 10]
            } else if (seedStart < sourceStart && seedEnd <= sourceEnd && seedEnd >= sourceStart && !hasBeenMapped) {
                long long firstRange = sourceStart - seedStart;
                insertLLongArray(&seedRanges, seedStart);
                insertLLongArray(&seedRanges, firstRange);
                insertLLongArray(&seedRanges, hasBeenMapped);

                insertLLongArray(&seedRanges, destStart);
                insertLLongArray(&seedRanges, seedRange - firstRange);
                insertLLongArray(&seedRanges, 1);
            // Case 4: seed range starts within mapping range but ends outside of it.
            // Seed: [5, 20], Mapping: [100, 0, 10] -> [105, 5], [10, 10] 
            } else if (seedStart >= sourceStart && seedStart <= sourceEnd && seedEnd > sourceEnd && !hasBeenMapped) {
                long long firstRange = seedStart - sourceStart;
                insertLLongArray(&seedRanges, destStart + firstRange);
                insertLLongArray(&seedRanges, sourceRange - firstRange);
                insertLLongArray(&seedRanges, 1);

                insertLLongArray(&seedRanges, seedStart + (sourceRange - firstRange));
                insertLLongArray(&seedRanges, seedRange - (sourceRange - firstRange));
                insertLLongArray(&seedRanges, hasBeenMapped);
            // Case 5: seed range has no overlap with the mapping range.
            // Seed: [0, 10], Mapping: [100, 100, 100] -> [0, 10]
            } else {
                insertLLongArray(&seedRanges, seedStart);
                insertLLongArray(&seedRanges, seedRange);
                insertLLongArray(&seedRanges, hasBeenMapped);
            }
        }

        seeds.numItems = 0;
        for (int idx = 0; idx < seedRanges.numItems; idx += 1) {
            insertLLongArray(&seeds, seedRanges.data[idx]);
        }

        // Instead of freeing, just reset numItems to 0 and start from the top so we don't malloc too much
        seedRanges.numItems = 0;
    }

    long long minSeed = seeds.data[0];
    for (int idx = 0; idx < seeds.numItems; idx += 3) {
        minSeed = min(minSeed, seeds.data[idx]);
    }

    freeLLongArray(&seedRanges);
    freeLLongArray(&seeds);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", minSeed, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}


int main() {
    problem1();
    problem2();

    return 0;
}

/**
 * Seed Range Example:
 * 
 * seeds: 10 20
 * 
 * seeds->fertilizer:
 * 0 20 5
 * 90, 15, 5
 * 
 * seeds:
 * [10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29]
 * 
 * fertilizer-sources:
 * [20, 21, 22, 23, 24]
 * [15, 16, 17, 18, 19]
 * 
 * fertilizer-dests:
 * [0, 1, 2, 3, 4]
 * [90, 91, 92, 93, 94]
 * 
 * new-seeds: 
 * [10, 11, 12, 13, 14] + [90, 91, 92, 93, 94] + [0, 1, 2, 3, 4] + [25, 26, 27, 28, 29]
 * 
 * new-seed-ranges:
 * [10, 5], [90, 5], [0, 5], [25, 5]
 * 
 * Lowest seed = min(range starts) = 0
 * 
 * CASES:
 * 
 * 1. seed range entirely within mapping range = [new seed start based on dest, same length]
 * 2. seed range contains mapping range = 
 *      [original seed start, new length up until contained mapping],
 *      [new seed start based on dest, mapping range length],
 *      [original seed start, og range length - r1 and r2 lengths]
 * 3. first part out, second part of seed range in mapping range =
 *      [original seed start, length up until mapping start],
 *      [new seed start based on dest, remaining length of og range]
 * 4. first part of seed range in, second part out of mapping range =
 *      [new seed start base on dest, range up until mapping end],
 *      [original seed start, remaining length of og range]
 * 5. seed range entirely outside of mapping range = keep range as is
*/
