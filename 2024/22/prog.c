#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/map.c"
#include "../../utils/string.c"

// Number of lines * 10
#define INPUT_MAX_SIZE 17740

#define SECRET_LEVELS 2000
#define SECRET_MODULO 16777216

void problem1(char* inputFilePath) {
    /*
    The problem is asking us to perform a series of operations on a set of input numbers for 2000
    iterations, then summing those results together. (Obviously in set up of part 2).

    Nothing much to say here, I've done what the problem asks.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char input[INPUT_MAX_SIZE];
    size_t fileLen = fread(input, sizeof(char), INPUT_MAX_SIZE, inputFile);
    fclose(inputFile);

    int endParserIdx = 0, secretLevel = 0;
    long long secretSum = 0;
    long long number;
    while (endParserIdx < fileLen) {
        secretLevel = 0;
        number = parseNumber(input, endParserIdx, &endParserIdx);

        while (secretLevel < SECRET_LEVELS) {
            number = ((number * 64) ^ number) % SECRET_MODULO;
            number = ((number / 32) ^ number) % SECRET_MODULO;
            number = ((number * 2048) ^ number) % SECRET_MODULO;

            secretLevel += 1;
        }

        secretSum += number;
    }

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", secretSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    For each of the part 1 iterations, we store the one's digit of the input number after the set of operations, which
    is the "price" if we'd sell bananas at that point. Then for the sequence of prices, choose the best sequence of 4
    deltas that, when first found for all input sequences, results in the most bananas if you summed up the scores.

    In addition to what is done in part 1, for each loop for a secret number in the input, store the price (last digit)
    in an array. After storing the sequence of prices, go through the deltas of all the prices, storing the 4 deltas
    and the value it lands you at in a hash-map, which sums up ALL first encounters of that sequence across ALL
    inputs.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char input[INPUT_MAX_SIZE];
    size_t fileLen = fread(input, sizeof(char), INPUT_MAX_SIZE, inputFile);
    fclose(inputFile);

    // Only the first appearance of the delta sequence can count, so we need to store if
    // we've seen the sequence before for the input being processed.
    long long encounteredThrowaway;
    LLongMap encounteredSequence;
    initLLongMap(&encounteredSequence);

    // A map of a delta sequence and the sum so far of all the prices for that sequence, across
    // all inputs.
    LLongMap sequenceSum;
    initLLongMap(&sequenceSum);

    // The price sequence for a single input number.
    IntArray sequence;
    initIntArray(&sequence, SECRET_LEVELS);

    // For each input number, perform the 2000 iterations, storing the price from each iteration in
    // the sequence list. Then, get the deltas from the sequence, and add the price that each
    // sequence yields to the sum of all prices for that sequence.
    int endParserIdx = 0, secretLevel = 0;
    long long number;
    while (endParserIdx < fileLen) {
        // Reset
        secretLevel = 0;
        sequence.numItems = 0;
        freeLLongMap(&encounteredSequence);

        number = parseNumber(input, endParserIdx, &endParserIdx);

        // Store the price of the first input (pre-operations).
        insertIntArray(&sequence, number % 10);
        while (secretLevel < SECRET_LEVELS) {
            number = ((number * 64) ^ number) % SECRET_MODULO;
            number = ((number / 32) ^ number) % SECRET_MODULO;
            number = ((number * 2048) ^ number) % SECRET_MODULO;

            insertIntArray(&sequence, number % 10);

            secretLevel += 1;
        }

        // Sum the sequence delta prices.
        long long cacheValue;
        int diff1, diff2, diff3, diff4, value;
        for (int idx = 4; idx < sequence.numItems - 1; idx += 1) {
            diff1 = sequence.data[idx - 3] - sequence.data[idx - 4];
            diff2 = sequence.data[idx - 2] - sequence.data[idx - 3];
            diff3 = sequence.data[idx - 1] - sequence.data[idx - 2];
            diff4 = sequence.data[idx] - sequence.data[idx - 1];
            value = sequence.data[idx];

            char* key = malloc(25);
            sprintf(key, "%d,%d,%d,%d", diff1, diff2, diff3, diff4);

            // If we've already encountered this sequence, skip it.
            if (getLLongMap(&encounteredSequence, key, &encounteredThrowaway)) continue;
            setLLongMap(&encounteredSequence, key, 1);

            // Add the price to the sum for that sequence.
            if (!getLLongMap(&sequenceSum, key, &cacheValue)) {
                setLLongMap(&sequenceSum, key, value);
            } else {
                setLLongMap(&sequenceSum, key, cacheValue + value);
            }
        }
    }

    // Get the max value from the sequence cache.
    long long maxValue = -1;
    for (int idx = 0; idx < sequenceSum.capacity; idx += 1) {
        if (sequenceSum.entries[idx].key == NULL) continue;

        maxValue = sequenceSum.entries[idx].value > maxValue ? sequenceSum.entries[idx].value : maxValue;
    }

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", maxValue, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
