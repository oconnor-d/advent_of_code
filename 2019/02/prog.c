#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/intcode.c"

#define OUTPUT 19690720

void problem1(char* inputFilePath) {
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);
    // printIntCodeProgram(&program);

    // Restore the program to the "1202 program alarm".
    program.program[1] = 12;
    program.program[2] = 2;

    intcodeRun(&program);

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", program.memory[0], (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

int nounVerbOutput(IntCodeProgram* program) {
    for (int noun = 0; noun < 99; noun += 1) {
        for (int verb = 0; verb < 99; verb += 1) {
            // Set the noun/verb.
            program->program[1] = noun;
            program->program[2] = verb;

            // Run the program.
            intcodeRun(program);

            // If we've got the right output, jump out of the nested for loop.
            if (program->memory[0] == OUTPUT) return 100 * noun + verb;
        }
    }

    return -1;
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    int output = nounVerbOutput(&program);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", output, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
