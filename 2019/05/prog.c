#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/intcode.c"

void problem1(char* inputFilePath) {
    clock_t start = clock();

    // Load the program.
    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    // Add the input.
    insertIntArray(&program.input, 1);

    // Run the program.
    intcodeRun(&program);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", program.output.data[program.output.numItems - 1], (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    // Load the program.
    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    // Add the input.
    insertIntArray(&program.input, 5);

    // Run the program.
    intcodeRun(&program);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", program.output.data[program.output.numItems - 1], (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
