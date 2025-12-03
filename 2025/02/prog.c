#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/math.c"
#include "../../utils/string.c"

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    lineLen = getline(&line, &lineCap, inputFile);

    long rangeStart, rangeEnd;
    int endIdx = -1;
    int numDigits;

    long invalidIDSum = 0;

    while (endIdx < lineLen - 1) {
        rangeStart = parseNumber(line, endIdx + 1, &endIdx);
        rangeEnd = parseNumber(line, endIdx + 1, &endIdx);

        for (long id = rangeStart; id <= rangeEnd; id += 1) {
            numDigits = countDigits(id);

            // All odd IDs are valid.
            if (numDigits % 2 == 0 && leftHalf(id, numDigits) == rightHalf(id, numDigits)) invalidIDSum += id;
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %ld [%.2fms]\n", invalidIDSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    lineLen = getline(&line, &lineCap, inputFile);

    long rangeStart, rangeEnd;
    int endIdx = -1;
    int numDigits;

    long invalidIDSum = 0;

    while (endIdx < lineLen - 1) {
        rangeStart = parseNumber(line, endIdx + 1, &endIdx);
        rangeEnd = parseNumber(line, endIdx + 1, &endIdx);

        for (long id = rangeStart; id <= rangeEnd; id += 1) {
            numDigits = countDigits(id);

            // For all factors of the ID, check if ALL factor-sized portions of the number are equal.
            // So for, 232323, check:
            //  Factor of 1: 2, 3, 2, 3, 2, 3 -> not equal.
            //  Factor of 2: 23, 23, 23 -> all equal (short-circuit, no need to check factor 3).
            // For a number like 123456, we'd also check:
            //  Factor of 3: 123, 456 -> not equal.
            for (int factor = 1; factor <= numDigits / 2; factor += 1) {
                // Continue for non-factors.
                if (numDigits % factor != 0) continue;

                // Set the divisor to 10^factor.
                int divisor = 1;
                for (int idx = 0; idx < factor; idx += 1) divisor *= 10;

                // For each factor-sized portion, compare it to the previous portion. If any aren't equal, we
                // don't have a repeating sequence.
                bool isValid = false;
                long lastIdPortion = id % divisor;
                long idPortion = id;
                while (idPortion > 0) {
                    if ((idPortion % divisor) != lastIdPortion) {
                        isValid = true;
                        break;
                    }

                    lastIdPortion = idPortion % divisor;
                    idPortion /= divisor;
                }

                if (!isValid) {
                    invalidIDSum += id;
                    // Don't want to check the other factors when we already know this ID is
                    // invalid.
                    break;
                }
            }
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %ld [%.2fms]\n", invalidIDSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
