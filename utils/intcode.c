/*
An Intcode Computer.
*/

#include <stdio.h>

#include "array.c"
#include "string.c"

typedef struct {
    int* program;
    int* memory;

    int size;
    int instructionPointer;
} IntCodeProgram;

// Add the numbers indexed by the following two numbers, and store the result at the index
// given by the third number.
#define ADD 1
// Multiply the numbers indexed by the following two numbers, and store the result at the index
// given by the third number.
#define MULTIPLY 2
// Immediately exit the Intcode program.
#define EXIT 99

void printIntCodeProgram(IntCodeProgram* program) {
    /*
    Nicely displays an intcode program and it's memory.
    */
    printf("[Size=%d, Instruction Pointer=%d]\n", program->size, program->instructionPointer);
    printf("Program: [");
    for (int idx = 0; idx < program->size; idx += 1) {
        if (idx == program->size - 1)
            printf("%d", program->program[idx]);
        else
            printf("%d, ", program->program[idx]);
    }
    printf("]\n");
    printf("Memory:  [");
    for (int idx = 0; idx < program->size; idx += 1) {
        if (idx == program->size - 1)
            printf("%d", program->memory[idx]);
        else
            printf("%d, ", program->memory[idx]);
    }
    printf("]\n\n");
}

void initIntCodeProgramFromIntArray(IntCodeProgram* program, IntArray* array) {
    /*
    Initializes an intcode program from the given integer array.
    */
    program->program = malloc(array->numItems * sizeof(int));
    for (int idx = 0; idx < array->numItems; idx += 1) program->program[idx] = array->data[idx];
    program->memory = malloc(array->numItems * sizeof(int));

    program->size = array->numItems;
    program->instructionPointer = 0;
}

void initIntCodeProgramFromFile(IntCodeProgram* program, char* inputFilePath) {
    /*
    Initializes an intcode program from a file containing the program as a comma-separated list
    of integers, like: 1, 2, 0, 15, 23, ...
    */
    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    IntArray input;
    initIntArray(&input, 500);
    lineLen = getline(&line, &lineCap, inputFile);

    int parserEndIdx = 0;
    while (parserEndIdx < lineLen - 1) insertIntArray(&input, parseNumber(line, parserEndIdx, &parserEndIdx));
    fclose(inputFile);

    initIntCodeProgramFromIntArray(program, &input);
}

void freeIntCodeProgram(IntCodeProgram* program) {
    free(program->program);
    program->program = NULL;
    free(program->memory);
    program->memory = NULL;

    program->size = 0;
    program->instructionPointer = 0;
}

void intcodeRun(IntCodeProgram* program) {
    /*
    Runs the given intcode program, the end result is stored in the program's memory, while the original
    program is preserved.

    Before running, copies the program into memory, overriding any previous memory the program may have.
    */

    // Copy the program into memory.
    for (int idx = 0; idx < program->size; idx += 1) program->memory[idx] = program->program[idx];

    int opcode;
    // Potential opcode parameters.
    int a, b, c;
    // Start from the beginning.
    program->instructionPointer = 0;
    while (program->instructionPointer >= 0 && program->instructionPointer < program->size) {
        opcode = program->memory[program->instructionPointer];

        if (opcode == ADD) {
            a = program->memory[program->instructionPointer + 1];
            b = program->memory[program->instructionPointer + 2];
            c = program->memory[program->instructionPointer + 3];

            program->memory[c] = program->memory[a] + program->memory[b];

            program->instructionPointer += 4;
        } else if (opcode == MULTIPLY) {
            a = program->memory[program->instructionPointer + 1];
            b = program->memory[program->instructionPointer + 2];
            c = program->memory[program->instructionPointer + 3];

            program->memory[c] = program->memory[a] * program->memory[b];

            program->instructionPointer += 4;
        } else if (opcode == EXIT) {
            // Immediately halt the program.
            return;
        }
    }
}
