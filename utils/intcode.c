/*
An Intcode Computer.
*/

#include <stdio.h>

#include "array.c"
#include "string.c"

// ================================ Constants ================================

// The max number of parameters any opcode has.
#define MAX_OPCODE_PARAMETERS 3

/*
The mode an opcode's parameters should be evaluated in.

Parameter Modes for each parameter for an opcode is stored in the same value as the opcode.
The right-most two digits in the instruction are the opcode, and going left each digit is
the mode of the parameter. Going left after the opcode, each digit is the mode of the next
parameter. If there's no digit for a parameter, the mode defaults to 0.

Example:

ABCDE
 1002

DE - two-digit opcode,      02 == opcode 2
 C - mode of 1st parameter,  0 == position mode
 B - mode of 2nd parameter,  1 == immediate mode
 A - mode of 3rd parameter,  0 == position mode,
                                  omitted due to being a leading zero
*/
typedef enum ParameterMode {
    POSITION = 0,
    IMMEDIATE = 1,
} ParameterMode;

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

// clang-format off
// The lengths of each instruction's parameter list, not including the opcode itself.
int INSTRUCTION_PARAMETER_LENGTHS[100] = {
    0, 3, 3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// The index of an opcode's output parameter, if it has one.
int INSTRUCTION_OUTPUT_PARAMETER_INDEXES[100] = {
    -1, 2, 2, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};
// clang-format on

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
        if (idx == program->instructionPointer) printf("->");
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

int getOpcode(IntCodeProgram* program, ParameterMode* parameterModes) {
    /*
    Gets the opcode at the current instruction pointer of the program, storing the instruction's parameter
    modes in the given array at the same time.

        [1, 0, 0, 1, ->2102, ...] -> opcode=2, parameterModes=[1, 2, 0]

    The instruction pointer MUST be pointing at an opcode.
    */
    int instruction = program->memory[program->instructionPointer];

    // If the instruction is only one or two digits, the opcode is the instruction and the parameter
    // modes are the implicit POSITION mode.
    if (instruction < 100) {
        for (int idx = 0; idx < MAX_OPCODE_PARAMETERS; idx += 1) parameterModes[idx] = POSITION;
        return instruction;
    }

    int opcode = instruction % 100;

    // Exclude the opcode from the instruction.
    instruction /= 100;

    // Load the parameter modes into the array.
    int numDigits = MAX_OPCODE_PARAMETERS - 1;
    while (numDigits >= 0) {
        // Even though we're getting the digits from the instruction going forward, we want to store it
        // going backward, so the mode for parameter 1 is at index 0, 2 is at index 1, etc.
        parameterModes[MAX_OPCODE_PARAMETERS - 1 - numDigits] = (ParameterMode)instruction % 10;

        instruction /= 10;
        numDigits -= 1;
    }

    return opcode;
}

int resolveNextInstruction(IntCodeProgram* program, ParameterMode* parameterModes, int* parameters) {
    /*
    Resolves the instruction the pointer is currently on, getting the opcode and storing the mode-evaluated
    parameters for the opcode in the parameter array.

    Advances the instruction pointer past this instruction and it's parameters, to the next instruction
    (or out of bounds if the instruction resolved was the last one in the program).

    The instruction pointer MUST be pointing at an opcode to start.
    */
    int opcode = getOpcode(program, parameterModes);
    program->instructionPointer += 1;

    for (int idx = 0; idx < INSTRUCTION_PARAMETER_LENGTHS[opcode]; idx += 1) {
        if (INSTRUCTION_OUTPUT_PARAMETER_INDEXES[opcode] == idx) {
            // Return the actual output index, since the opcode handles storing it.
            parameters[idx] = program->memory[program->instructionPointer];
        } else if (parameterModes[idx] == POSITION) {
            parameters[idx] = program->memory[program->memory[program->instructionPointer]];
        } else if (parameterModes[idx] == IMMEDIATE) {
            parameters[idx] = program->memory[program->instructionPointer];
        }

        program->instructionPointer += 1;
    }

    return opcode;
}

void intcodeRun(IntCodeProgram* program) {
    /*
    Runs the given intcode program, the end result is stored in the program's memory, while the original
    program is preserved.

    Before running, copies the program into memory, overriding any previous memory the program may have.
    */

    // Copy the program into memory.
    for (int idx = 0; idx < program->size; idx += 1) program->memory[idx] = program->program[idx];

    // The opcode being processed.
    int opcode;
    // Potential opcode parameters and the modes they were evaluated in.
    ParameterMode parameterModes[MAX_OPCODE_PARAMETERS];
    int parameters[MAX_OPCODE_PARAMETERS];
    // Start from the beginning.
    program->instructionPointer = 0;
    while (program->instructionPointer >= 0 && program->instructionPointer < program->size) {
        // Resolve the next instruction - getting the opcode, storing the parameter values already evaluated
        // as per their mode, and advancing the instruction pointer to the start of the next instruction.
        opcode = resolveNextInstruction(program, parameterModes, parameters);

        if (opcode == ADD) {
            program->memory[parameters[2]] = parameters[0] + parameters[1];
        } else if (opcode == MULTIPLY) {
            program->memory[parameters[2]] = parameters[0] * parameters[1];
        } else if (opcode == EXIT) {
            // Immediately halt the program.
            return;
        }
    }
}
