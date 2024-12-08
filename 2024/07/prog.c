#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

long long concatenate(long long x, long long y) {
    long long magnitude = 10;
    while (y >= magnitude)
        magnitude *= 10;
    return (x * magnitude) + y;
}

void problem1(char* inputFilePath) {
    /*
    The problem is asking us to figure out valid configurations of + and/or * in an equation to achieve
    a certain result. Normal order of opreations doesn't apply, it's just evaluated left to right.

        190: 5 5 19 has a valid configuration of 5 + 5 * 19, for example.

    This generates all possible operator combinations by keeping track of all previous possible combinations,
    and applying both + and * for the current number to all of the previous combinations, like:

        191: 5 5 19 1 -> [10, 25 | 29, 190, 44, 456 | 30, 29, 191, 190, 45, 44, 457, 456]

    and if the result is in the last iteration, we've configured a valid equation.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    long long validResultSum = 0, result = 0;

    IntArray numbers;
    LLongArray prevCalcs;
    initIntArray(&numbers, 20);
    initLLongArray(&prevCalcs, 100);
    int parserEndIdx, prevNumItems, prevCalcIdx;
    long long sum, product;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        numbers.numItems = 0;
        prevCalcs.numItems = 0;
        result = parseNumber(line, 0, &parserEndIdx);

        // Parse the numbers into an array for easy access.
        while (parserEndIdx < lineLen - 1) insertIntArray(&numbers, parseNumber(line, parserEndIdx, &parserEndIdx));

        // For each number, store the calculation of adding AND multiplying it with the results of ALL previous additions
        // and multiplications. Handle the first idx as an edge case, adding and multiplying the first tow numbers and skipping
        // along to the third for the normal algorithm.
        //
        // If the numbers are [1, 2, 3], a first iteration will store the calculations:
        //  [1 + 2, 1 * 2]
        // and the next iteration will than store the calculations of adding AND multiplying three to each of those:
        //  [1 + 2, 1 * 2] + [1 + 2 + 3, 1 + 2 * 3, 1 * 2 + 3, 1 * 2 * 3]
        // and so on. However, if a calculation EXCEEDS the result, we know it's invalid and don't store it in the
        // prev calcs array, since any future operation is only ever going to make the number greater, since there's
        // no subtraction or division.
        //
        // We keep the older calculations in the list and just keep track of we're the previous iteration left us, mostly
        // for convenience.
        //
        // prevCalcIdx stores where we're currently at in the calculations array, and prevSize stores the size of the last
        // batch of calculations performed -> 2, 4, 8, etc.
        prevCalcIdx = 0;
        for (int idx = 0; idx < numbers.numItems; idx += 1) {
            // Edge case - kickstart the calculations by adding the first two numbers together, then skip to the third number.
            if (idx == 0) {
                insertLLongArray(&prevCalcs, numbers.data[idx] + numbers.data[idx + 1]);
                insertLLongArray(&prevCalcs, numbers.data[idx] * numbers.data[idx + 1]);
                idx += 1;
                continue;
            }

            prevNumItems = prevCalcs.numItems;
            // For all previous calculations, store the +/* calculations using them for the current number.
            while (prevCalcIdx < prevNumItems) {
                sum = numbers.data[idx] + prevCalcs.data[prevCalcIdx];
                product = numbers.data[idx] * prevCalcs.data[prevCalcIdx];

                if (sum <= result) insertLLongArray(&prevCalcs, sum);
                if (product <= result) insertLLongArray(&prevCalcs, product);

                prevCalcIdx += 1;
            }
        }

        // Go through the final set of calculations, checking if any equal the result.
        while (prevCalcIdx < prevCalcs.numItems) {
            if (prevCalcs.data[prevCalcIdx] == result) {
                validResultSum += result;
                break;
            }

            prevCalcIdx += 1;
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", validResultSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Exact same as problem 1, except this time we calculate the third || operation on each iteration.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    long long validResultSum = 0, result = 0;

    IntArray numbers;
    LLongArray prevCalcs;
    initIntArray(&numbers, 20);
    initLLongArray(&prevCalcs, 100);
    int parserEndIdx, prevNumItems, prevCalcIdx;
    long long sum, product, concatenation;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        numbers.numItems = 0;
        prevCalcs.numItems = 0;
        result = parseNumber(line, 0, &parserEndIdx);

        // Parse the numbers into an array for easy access.
        while (parserEndIdx < lineLen - 1) insertIntArray(&numbers, parseNumber(line, parserEndIdx, &parserEndIdx));

        prevCalcIdx = 0;
        for (int idx = 0; idx < numbers.numItems; idx += 1) {
            if (idx == 0) {
                insertLLongArray(&prevCalcs, numbers.data[idx] + numbers.data[idx + 1]);
                insertLLongArray(&prevCalcs, numbers.data[idx] * numbers.data[idx + 1]);
                insertLLongArray(&prevCalcs, concatenate(numbers.data[idx], numbers.data[idx + 1]));
                idx += 1;
                continue;
            }

            prevNumItems = prevCalcs.numItems;
            while (prevCalcIdx < prevNumItems) {
                sum = prevCalcs.data[prevCalcIdx] + numbers.data[idx];
                product = prevCalcs.data[prevCalcIdx] * numbers.data[idx];
                concatenation = concatenate(prevCalcs.data[prevCalcIdx], numbers.data[idx]);

                if (sum <= result) insertLLongArray(&prevCalcs, sum);
                if (product <= result) insertLLongArray(&prevCalcs, product);
                if (concatenation <= result) insertLLongArray(&prevCalcs, concatenation);

                prevCalcIdx += 1;
            }
        }

        while (prevCalcIdx < prevCalcs.numItems) {
            if (prevCalcs.data[prevCalcIdx] == result) {
                validResultSum += result;
                break;
            }

            prevCalcIdx += 1;
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", validResultSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
