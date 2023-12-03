#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


void problem_1() {
    clock_t start = clock();

    FILE *inputFile = fopen("2023/01/input.txt", "r");

    int total = 0;
    int digit1, digit2;

    char *line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        digit1 = digit2 = -1;

        for (int idx = 0; idx < lineLen; idx += 1) {
            if (isdigit(line[idx]) && digit1 == -1) {
                digit1 = line[idx] - '0';
            } else if (isdigit(line[idx])) {
                digit2 = line[idx] - '0';
            }
        }

        total += (digit1 * 10) + ((digit2 == -1) ? digit1 : digit2);
    }

    clock_t end = clock();
    printf("Problem 01: %d [%fs]\n", total, (double)(end - start) / CLOCKS_PER_SEC);
}


void problem_2() {
    clock_t start = clock();

    FILE *inputFile = fopen("2023/01/input.txt", "r");

    int total = 0;
    int foundDigit, digit1, digit2;

    char *line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        foundDigit = digit1 = digit2 = -1;

        for (int idx = 0; idx < lineLen; idx += 1) {
            if (isdigit(line[idx])) {
                foundDigit = line[idx] - '0';
            } else if (strncmp("one", &line[idx], 3) == 0) {
                foundDigit = 1;
            } else if (strncmp("two", &line[idx], 3) == 0) {
                foundDigit = 2;
            } else if (strncmp("three", &line[idx], 5) == 0) {
                foundDigit = 3;
            } else if (strncmp("four", &line[idx], 4) == 0) {
                foundDigit = 4;
            } else if (strncmp("five", &line[idx], 4) == 0) {
                foundDigit = 5;
            } else if (strncmp("six", &line[idx], 3) == 0) {
                foundDigit = 6;
            } else if (strncmp("seven", &line[idx], 5) == 0) {
                foundDigit = 7;
            } else if (strncmp("eight", &line[idx], 5) == 0) {
                foundDigit = 8;
            } else if (strncmp("nine", &line[idx], 4) == 0) {
                foundDigit = 9;
            }

            if (foundDigit != -1 && digit1 == -1) {
                digit1 = foundDigit;
            } else if (foundDigit != -1) {
                digit2 = foundDigit;
            }
            foundDigit = -1;
        }

        total += (digit1 * 10) + ((digit2 == -1) ? digit1 : digit2);
    }

    clock_t end = clock();
    printf("Problem 02: %d [%fs]\n", total, (double)(end - start) / CLOCKS_PER_SEC);
}


int main() {
    problem_1();
    problem_2();

    return 0;
}
