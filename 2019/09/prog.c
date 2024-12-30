#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/intcode.c"

void problem1(char* inputFilePath) {
    /*
    All of this problem was handled by adding to and updating how the IntCode interpreter works.

        1. Introduce RELATIVE parameter mode.
        2. Introduce dynamic memory, increasing the memory if indexing out of bounds of the current
           allocated memory space.
        3. Handle large numbers. This meant refactoring intcode and previous problems to use long long
           and LLongArrays instead of ints and IntArrays.

    Once that was done, run the input program with value 1 and report it's only output.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    insertLLongArray(&program.input, 1);
    intcodeRun(&program);

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", program.output.data[program.output.numItems - 1], (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Same as part 1, but with 2 as the input value.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    insertLLongArray(&program.input, 2);
    intcodeRun(&program);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", program.output.data[program.output.numItems - 1], (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
