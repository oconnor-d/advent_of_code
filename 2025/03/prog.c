#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void problem1(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    long joltage = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        char digit1 = line[0], digit2;
        int digit1Index = 0;

        // Can never use the last digit.
        for (int idx = 1; idx < lineLen - 2; idx += 1) {
            if (line[idx] > digit1) {
                digit1 = line[idx];
                digit1Index = idx;
            }
        }

        digit2 = line[digit1Index + 1];
        for (int idx = digit1Index + 1; idx < lineLen; idx += 1) {
            if (line[idx] > digit2) {
                digit2 = line[idx];
            }
        }

        joltage += (digit1 - '0') * 10 + (digit2 - '0');
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %ld [%.2fms]\n", joltage, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    long long joltage = 0;
    long partialJoltage;
    int lastIndex;
    char bestDigit;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        lastIndex = 0;
        bestDigit = line[0];

        partialJoltage = 0;

        // For each digit we need to find, get the greatest left-most digit that still leaves
        // room for the remaining digits needed to fill out all 12 battery slots.
        for (int idx = 0; idx < 12; idx += 1) {
            for (int dIdx = lastIndex + 1; dIdx < (lineLen - 12 + idx); dIdx += 1) {
                if (line[dIdx] > bestDigit) {
                    bestDigit = line[dIdx];
                    lastIndex = dIdx;
                }
            }

            // Build up the partialJoltage digit by digit. The repeated "* 10"'s means that the first
            // digit found will end up as the most significant, the last one will be in the one's spot.
            partialJoltage = (partialJoltage * 10) + (bestDigit - '0');

            // Set up the first digit to compare against for the next index.
            lastIndex += 1;
            bestDigit = line[lastIndex];
        }

        joltage += partialJoltage;
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", joltage, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
