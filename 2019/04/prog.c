#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/string.c"

void getDigits(int number, int numDigits, int* output) {
    /*
    Loads the individual digits of the number into the output.

        123456 -> [1, 2, 3, 4, 5, 6]
    */
    numDigits -= 1;
    while (numDigits >= 0) {
        output[numDigits] = number % 10;

        number /= 10;
        numDigits -= 1;
    }
}

void problem1(char* inputFilePath) {
    /*
    Count valid passwords in a range. A valid password:
        - is only ascending (i.e. 12345)
        - has at least two consecutive digits that are the same (i.e. 1222345)

    Since there's not a lot of passwords in the input range, I generate all candidate passwords
    and check them against the above two constraints.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;

    int parserEndIdx;
    int lower, upper;
    getline(&line, &lineCap, inputFile);
    fclose(inputFile);

    lower = parseNumber(line, 0, &parserEndIdx);
    upper = parseNumber(line, parserEndIdx + 1, &parserEndIdx);

    int password = lower;
    int validPasswords = 0;
    int* digits = malloc(6 * sizeof(int));

    // Test the validity of every candidate password in the range.
    bool allAscending, consecutiveDigits;
    while (password <= upper) {
        getDigits(password, 6, digits);

        allAscending = true;
        consecutiveDigits = false;
        for (int idx = 1; idx < 6; idx += 1) {
            if (digits[idx - 1] == digits[idx]) consecutiveDigits = true;
            if (digits[idx - 1] > digits[idx]) allAscending = false;
        }

        if (consecutiveDigits && allAscending) validPasswords += 1;

        password += 1;
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", validPasswords, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Like part 1, but the password must have exactly at least one set of two consecutive digits.

    This is valid: 11122333, because 22.
    This is invalid: 111345, because 111 has three consecutive digits.

    The algorithm is the same, but this time the count of consecutive digits are counted.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;

    int parserEndIdx;
    int lower, upper;
    getline(&line, &lineCap, inputFile);
    fclose(inputFile);

    lower = parseNumber(line, 0, &parserEndIdx);
    upper = parseNumber(line, parserEndIdx + 1, &parserEndIdx);

    int password = lower;
    int validPasswords = 0;
    int* digits = malloc(6 * sizeof(int));
    int consecutiveCounts[10];

    bool allAscending, consecutiveDigits;
    while (password <= upper) {
        getDigits(password, 6, digits);

        // Reset the consecutive counts, preload the first digit since the below loop
        // skips over it.
        for (int idx = 0; idx < 10; idx += 1) consecutiveCounts[idx] = 0;
        consecutiveCounts[digits[0]] += 1;

        allAscending = true;
        for (int idx = 1; idx < 6; idx += 1) {
            consecutiveCounts[digits[idx]] += 1;
            if (digits[idx - 1] > digits[idx]) allAscending = false;
        }

        consecutiveDigits = false;
        for (int digit = 1; digit < 10; digit += 1) {
            if (consecutiveCounts[digit] == 2) consecutiveDigits = true;
        }

        if (consecutiveDigits && allAscending) validPasswords += 1;

        password += 1;
    }

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", validPasswords, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
