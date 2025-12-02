#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/string.c"

#define LINE_BUFFER 1000

// TODO: move to math.c
int countDigits(long number) {
    /*
    Counts the number of digits in the given number.
    */
    if (number < 0) number *= -1;

    int digits = 1;
    while (number > 9) {
        number /= 10;
        digits += 1;
    }

    return digits;
}

long rightHalf(long number, int digits) {
    /*
    Gets the right half of the number. The number of digits in the number
    MUST be even, otherwise this will not work reliably.
    */
    // Manually compute power since `pow` doesn't return an int.
    int power = 10;
    for (int idx = 1; idx < digits / 2; idx += 1) {
        power *= 10;
    }

    return number % power;
}

long leftHalf(long number, int digits) {
    /*
    Gets the left half of the number. The number of digits in the number
    MUST be even, otherwise this will not work reliably.
    */
    // Manually compute power since `pow` doesn't return an int.
    int power = 10;
    for (int idx = 1; idx < digits / 2; idx += 1) {
        power *= 10;
    }

    return number / power;
}

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    lineLen = getline(&line, &lineCap, inputFile);

    long long rangeStart, rangeEnd;
    int endIdx = -1;
    int numDigits;

    long long invalidIDSum = 0;

    while (endIdx < lineLen - 1) {
        rangeStart = parseNumber(line, endIdx + 1, &endIdx);
        rangeEnd = parseNumber(line, endIdx + 1, &endIdx);

        for (long long id = rangeStart; id <= rangeEnd; id += 1) {
            numDigits = countDigits(id);

            // All odd IDs are valid.
            if (numDigits % 2 == 1) continue;

            if (leftHalf(id, numDigits) == rightHalf(id, numDigits)) {
                // printf("VALID: %lld\n", id);
                invalidIDSum += id;
            }
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", invalidIDSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    lineLen = getline(&line, &lineCap, inputFile);

    long long rangeStart, rangeEnd;
    int endIdx = -1;
    int numDigits;

    long long invalidIDSum = 0;

    while (endIdx < lineLen - 1) {
        rangeStart = parseNumber(line, endIdx + 1, &endIdx);
        rangeEnd = parseNumber(line, endIdx + 1, &endIdx);

        for (long long id = rangeStart; id <= rangeEnd; id += 1) {
            numDigits = countDigits(id);

            // For all factors of the ID, check if ALL factor-sized portions of the number are equal.
            // So for, 232323, check:
            //  Factor of 1: 2, 3, 2, 3, 2, 3 -> not equal.
            //  Factor of 2: 23, 23, 23 -> all equal (short-circuit, no need to check factor 3).
            // For a number like 123456, we'd also check:
            //  Factor of 3: 123, 456 -> not equal.
            for (int factor = 0; factor <= numDigits / 2; factor += 1) {
                // Continue for non-factors.
                if (numDigits % factor != 0) continue;

                // Set the divisor to 10^factor.
                int divisor = 1;
                for (int idx = 0; idx < factor; idx += 1) divisor *= 10;

                // For each factor-sized portion, compare it to the previous portion. If any aren't equal, we
                // don't have a repeating sequence.
                int matches = 1;
                long long lastIdPortion = id % divisor;
                long long idPortion = id;
                while (idPortion > 0) {
                    if ((idPortion % divisor) != lastIdPortion) {
                        matches = 0;
                        break;
                    }

                    lastIdPortion = idPortion % divisor;
                    idPortion /= divisor;
                }

                if (matches) {
                    invalidIDSum += id;
                    break;
                }
            }
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", invalidIDSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
