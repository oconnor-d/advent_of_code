#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/string.c"

void problem1(char* inputFilePath) {
    /*
    Get the sum of fuel required for a list of modules, by their mass.

    The fuel formula is mass / 3 - 2.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int parserEndIdx;
    int moduleMass, fuel = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        moduleMass = parseNumber(line, 0, &parserEndIdx);

        fuel += moduleMass / 3 - 2;
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", fuel, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Get the sum of fuel required for a list of modules, by their mass, AND the fuel required
    for the fuel itself, using the same formula. Negative fuel is free.

    The fuel formula is mass / 3 - 2.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int parserEndIdx;
    int moduleMass, fuel = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        moduleMass = parseNumber(line, 0, &parserEndIdx);

        // Keep adding the fuel mass while the mass is positive.
        while (moduleMass > 0) {
            moduleMass = moduleMass / 3 - 2;
            fuel += moduleMass;
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", fuel, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
