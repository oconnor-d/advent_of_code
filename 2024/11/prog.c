#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/map.c"
#include "../../utils/math.c"
#include "../../utils/string.c"

#define INPUT_MAX_SIZE 50

void problem1(char* inputFilePath) {
    /*
    Each time you blink, the number-engraved rocks will change or proliferate based on this set of rules, all
    simultaneously:
    1. If the rock's number is 0, it becomes 1.
    2. If the rock's number's number of digits is even, it gets split into two rocks, one with the
        first half of the digits and one with the second half.
    3. Otherwise, the rock's number gets multiplied by 2024.

    The problem is asking for the count of rocks there would be from a list of starting rocks after 25 blinks.

    Pretty simply, we go through the list of rocks 25 times, applying the rules and using the resultant
    rocks as the next blink's set of rocks.

    We could speed this up simply by using part 2's code for 25 blinks, but I'm keeping the original solution
    for posterity.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char input[INPUT_MAX_SIZE];
    size_t fileLen = fread(input, sizeof(char), INPUT_MAX_SIZE, inputFile);
    fclose(inputFile);

    // Parse the rocks into an array.
    LLongArray rocks1;
    initLLongArray(&rocks1, 1000);

    int parserEndIdx = 0;
    while (parserEndIdx < fileLen - 1) insertLLongArray(&rocks1, parseNumber(input, parserEndIdx, &parserEndIdx));

    // Since we don't want to check the rock array from under our feet as we're processing the rocks, we'll loop
    // through one array of rocks `rocks` and store the results in a different array `newRocks`, and whenever we
    // get to the next blink will swap the two arrays, so that for the next iteration the `rocks` will hold
    // the values of `newRocks`, and we're good to use `newRocks` for the next set of changes.
    LLongArray rocks2, *temp = NULL, *rocks = NULL, *newRocks = NULL;
    initLLongArray(&rocks2, 1000);

    rocks = &rocks1;
    newRocks = &rocks2;

    int BLINK_COUNT = 25;

    // Apply the rules BLINK_COUNT amount of times.
    int blinks = 0, digits;
    while (blinks < BLINK_COUNT) {
        blinks += 1;

        // Reset newRocks so we insert at the beginning.
        newRocks->numItems = 0;

        // Apply the rules to each rock, and store the resultant rock(s) in the new array.
        for (int idx = 0; idx < rocks->numItems; idx += 1) {
            digits = countDigits(rocks->data[idx]);

            if (rocks->data[idx] == 0) {
                insertLLongArray(newRocks, 1);
            } else if (digits % 2 == 0) {
                insertLLongArray(newRocks, leftHalf(rocks->data[idx], digits));
                insertLLongArray(newRocks, rightHalf(rocks->data[idx], digits));
            } else {
                insertLLongArray(newRocks, rocks->data[idx] * 2024);
            }
        }

        // Swap the pointers, so that `rocks` has the updated set of rocks for the next iteration.
        temp = rocks;
        rocks = newRocks;
        newRocks = temp;
    }

    // After the loop is done, `rocks` now holds the updated set of rocks after BLINK_COUNT blinks.

    clock_t end = clock();
    printf("Problem 01: %zu [%.2fms]\n", rocks->numItems, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

long long stonesAfterBlinks(long long stone, int blinks, LLongMap* cache) {
    /*
    Gets the number of stones the given stone will turn into after the given number of blinks.

    This function works by recursively getting the number of rocks this rock's alteration(s) will turn into
    after 1 less blink, and uses the cache to prevent long computations of numbers we've already seen.

    The cache stores the resultant number of rocks, keyed by "<stone_number>-<blinks>". We use the composite
    key instead of a map of maps because I only have a str -> long long map implemented.
    */

    // Not blinking = no new stones are added.
    if (blinks == 0) return 1;

    int digits = countDigits(stone);

    // Create the "<stone_number>-<blinks>" key for map lookup and insertion.
    char* key = malloc(25);
    sprintf(key, "%lld-%d", stone, blinks);

    // If we have the number of stones this stone will turn into after the current number of blinks, just
    // return that.
    long long nextStones;
    if (getLLongMap(cache, key, &nextStones)) return nextStones;

    // Otherwise, get the total number of stones this stone will turn into after applying the rock-changing rule,
    // while also "consuming" the current blink.
    if (stone == 0) {
        nextStones = stonesAfterBlinks(1, blinks - 1, cache);
    } else if (digits % 2 == 0) {
        nextStones = stonesAfterBlinks(leftHalf(stone, digits), blinks - 1, cache) + stonesAfterBlinks(rightHalf(stone, digits), blinks - 1, cache);
    } else {
        nextStones = stonesAfterBlinks(stone * 2024, blinks - 1, cache);
    }

    // Store the result in the cache for future lookup.
    setLLongMap(cache, key, nextStones);

    return nextStones;
}

void problem2(char* inputFilePath) {
    /*
    This problem is the same as problem 1, but for 75 blinks. Part 1's solution would be much to slow for 75 blinks,
    and the number of rocks we'd need to store in an array would be huge, so this gets the count of rocks a
    completely different way.

    For starters, we don't store the altered rocks in an array anymore, it would be too big.

    Secondly, we cache the number of rocks a given rock will turn in to after N blinks. A lot of the numbers that show
    up after altercations are numbers we've processed before, so we use our previous work.

    Finally, since the rocks in the array don't affect each other, the total amount of stones in an array of rocks after
    N blinks is the same as the sum of each rock in the array after N blinks, so we can get the stone counts of each
    rock individually, and then add them all up.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char input[INPUT_MAX_SIZE];
    size_t fileLen = fread(input, sizeof(char), INPUT_MAX_SIZE, inputFile);
    fclose(inputFile);

    // Parse the rocks into an array.
    LLongArray rocks;
    initLLongArray(&rocks, 10);

    int parserEndIdx = 0;
    while (parserEndIdx < fileLen - 1) insertLLongArray(&rocks, parseNumber(input, parserEndIdx, &parserEndIdx));
    fclose(inputFile);

    int BLINK_COUNT = 75;

    // This cache will hold the number of rocks a given rock turns into after a certain number of blinks.
    LLongMap cache;
    initLLongMap(&cache);

    // For each rock in the input, get the number of stones after BLINK_COUNT blinks, and sum them together.
    long long totalStones = 0;
    for (int idx = 0; idx < rocks.numItems; idx += 1) {
        totalStones += stonesAfterBlinks(rocks.data[idx], BLINK_COUNT, &cache);
    }

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", totalStones, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
