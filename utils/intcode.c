/*
Interpreter for running Intcode Programs.

An Intcode program is a list of integers representing opcodes and parameters for the opcodes to operate
on. For example, 1,0,0,2,1,3,99 is an Intcode Program.

Refer to the struct and `intcodeRun` docs for more details on how an Intcode program works.
*/

#include <stdbool.h>
#include <stdio.h>

#include "array.c"
#include "string.c"

// ================================ Constants ================================

// The max number of parameters any opcode has.
#define MAX_OPCODE_PARAMETERS 3

// clang-format off
// The lengths of each instruction's parameter list, not including the opcode itself.
int INSTRUCTION_PARAMETER_LENGTHS[100] = {
    0, 3, 3, 1, 1, 2, 2, 3, 3, 0,
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
    -1, 2, 2, 0, 0, -1, -1, 2, 2, -1,
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

// ================================ Data ================================

typedef enum OpCode {
    // Adds the first two parameters, and store the result at the index
    // given by the third number.
    ADD = 1,
    // Multiplies the first two parameters, and store the result at the index
    // given by the third number.
    MULTIPLY = 2,
    // Stores the next input at the address of it's only parameter.
    STORE_INPUT = 3,
    // Outputs the value of it's only parameter.
    OUTPUT = 4,
    // Jumps to the address of the second parameter iff the first is non-zero.
    JUMP_IF_TRUE = 5,
    // Jumps to the address of the second parameter iff the first is zero.
    JUMP_IF_FALSE = 6,
    // Stores 1 in the third parameter address if the first is less than the second, 0 otherwise.
    LESS_THAN = 7,
    // Stores 1 in the third parameter address if the first is equal to the second, 0 otherwise.
    EQUALS = 8,
    // Immediately exit the program.
    EXIT = 99,
} OpCode;

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

/*
An Intcode Program, holding the source code, the program's working memory, where the program is at
in evaluation, I/O, and more.

Running:

In general, the source code is held in `program`, and gets copied over into `memory` before the
program is run. Once the program is run, `instructionPointer` points to the next instruction to be
evaluated.

If a program hits the EXIT opcode, it is marked as `halted`. There are times when the program stops
running without fully halting, like when it's waiting on input.

I/O:

The `input` and `output` buffers handle the program's I/O. The input stores all current input to the
program, and the `inputPointer` points to the next input to consume (like for a STORE_INPUT instruction).
The output stores all outputted, with the most recent output being the last element in the buffer.
*/
typedef struct {
    int* program;
    int* memory;

    int size;
    int instructionPointer;
    // If the program is halted, i.e. hit opcode 99 or has not yet been run, or not.
    bool halted;

    int inputPointer;
    IntArray input;
    IntArray output;
} IntCodeProgram;

// ================================ Functions ================================

void printIntCodeProgram(IntCodeProgram* program) {
    /*
    Nicely displays an intcode program and it's memory.
    */
    printf("[Size=%d, Instruction Pointer=%d, Halted=%d]\n", program->size, program->instructionPointer, program->halted);
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
    printf("]\n");

    printf("Input: ");
    printIntArray(&program->input);

    printf("Output: ");
    printIntArray(&program->input);
    printf("\n");
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
    // A never-before-run program starts off as halted, until it's run for the first time.
    program->halted = true;

    initIntArray(&program->input, 1024);
    initIntArray(&program->output, 1024);

    program->inputPointer = 0;
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
    /*
    Frees all memory associated with the program.
    */
    free(program->program);
    program->program = NULL;
    free(program->memory);
    program->memory = NULL;

    program->size = 0;
    program->instructionPointer = 0;

    freeIntArray(&program->input);
    freeIntArray(&program->output);
    program->inputPointer = 0;
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

    // If the program to run has halted, reset it to run from the beginning, otherwise, the program
    // will be run from the instruction pointer it left off at.
    if (program->halted) {
        // Copy the program into memory.
        for (int idx = 0; idx < program->size; idx += 1) program->memory[idx] = program->program[idx];

        // Reset pointers and output.
        program->inputPointer = 0;
        program->output.numItems = 0;
        program->instructionPointer = 0;
        program->halted = false;
    }

    // The opcode being processed.
    int opcode;
    // Potential opcode parameters and the modes they were evaluated in.
    ParameterMode parameterModes[MAX_OPCODE_PARAMETERS];
    int parameters[MAX_OPCODE_PARAMETERS];
    while (program->instructionPointer >= 0 && program->instructionPointer < program->size) {
        // Resolve the next instruction - getting the opcode, storing the parameter values already evaluated
        // as per their mode, and advancing the instruction pointer to the start of the next instruction.
        opcode = resolveNextInstruction(program, parameterModes, parameters);

        if (opcode == ADD) {
            program->memory[parameters[2]] = parameters[0] + parameters[1];
        } else if (opcode == MULTIPLY) {
            program->memory[parameters[2]] = parameters[0] * parameters[1];
        } else if (opcode == STORE_INPUT) {
            if (program->inputPointer >= program->input.numItems) {
                // Waiting on input. Reset the pointer to the start of this instruction and return early. The
                // next time the program is run it will resume from this instruction, potentially with new input.
                program->instructionPointer -= 2;
                return;
            }

            program->memory[parameters[0]] = program->input.data[program->inputPointer];
            program->inputPointer += 1;
        } else if (opcode == OUTPUT) {
            insertIntArray(&program->output, program->memory[parameters[0]]);
        } else if (opcode == JUMP_IF_TRUE) {
            if (parameters[0] != 0) program->instructionPointer = parameters[1];
        } else if (opcode == JUMP_IF_FALSE) {
            if (parameters[0] == 0) program->instructionPointer = parameters[1];
        } else if (opcode == LESS_THAN) {
            program->memory[parameters[2]] = parameters[0] < parameters[1] ? 1 : 0;
        } else if (opcode == EQUALS) {
            program->memory[parameters[2]] = parameters[0] == parameters[1] ? 1 : 0;
        } else if (opcode == EXIT) {
            // Immediately halt the program, and mark it as halted.
            program->halted = true;
            return;
        }
    }
}
