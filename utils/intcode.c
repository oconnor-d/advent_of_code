/*
Interpreter for running Intcode Programs.

An Intcode program is a list of integers representing opcodes and parameters for the opcodes to operate
on. For example, 1,0,0,2,1,3,99 is an Intcode Program.

Refer to the struct and `intcodeRun` docs for more details on how an Intcode program works.

To run an Intcode program from a file with custom input:

char* filePath = "test.intcode"

// Load the file into the program.
IntCodeProgram program;
initIntCodeProgramFromFile(&program, filePath);

// Pass in some input value.
pushInput(&program, 1);

// Run the program.
intcodeRun(&program);

// See any output.
printLLongArray(&program.output);

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
    0, 3, 3, 1, 1, 2, 2, 3, 3, 1,
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
    -1,  2,  2,  0, -1, -1, -1,  2,  2, -1,
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
    // Increases (or decreases if it's parameter is negative) the relative base for the RELATIVE parameter mode
    // by the value of it's only parameter.
    ADJUST_RELATIVE_BASE = 9,
    // Immediately exit the program.
    EXIT = 99,
} OpCode;

/*
The mode an opcode's parameters should be evaluated in.

Parameter Modes for each parameter for an opcode is stored in the same value as the opcode.
The right-most two digits in the instruction are the opcode, and going left each digit is
the mode of the parameter. Going left after the opcode, each digit is the mode of the next
parameter. If there's no digit for a parameter, the mode defaults to 0.

POSITION:
The parameter value resolves to the value in the parameter's index of memory. So in POSITION
parameter mode, 3 would resolve to program->memory[3].

IMMEDIATE:
The parameter value resolves to itself. So in IMMEDIATE parameter mode, 3 would resolve to 3.

RELATIVE:
The parameter value resolves to the value in the parameter's index of memory, plus the current
`relativeBase` of the program. The value of `relativeBase` is modified via other opcodes, like
ADJUST_RELATIVE_BASE. So in RELATIVE parameter mode with a current relative base of 7, 3 would
resolve to program->memory[10].

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
    RELATIVE = 2,
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

Memory:

The program's memory is initialized to the values of the program itself. As the program is run, the memory
is expanded as needed. When a new block of memory is allocated, all the values in it are initialized to
0. Accessing memory outside of the current allocated memory triggers the resize.

I/O:

The `input` and `output` buffers handle the program's I/O. The input stores all current input to the
program, and the `inputPointer` points to the next input to consume (like for a STORE_INPUT instruction).
The output stores all outputted, with the most recent output being the last element in the buffer.
*/
typedef struct {
    size_t programSize;
    long long* program;

    size_t memorySize;
    long long* memory;

    size_t instructionPointer;
    // If the program is halted, i.e. hit opcode 99 or has not yet been run, or not.
    bool halted;
    size_t relativeBase;

    size_t inputPointer;
    LLongArray input;
    LLongArray output;
} IntCodeProgram;

// ================================ Utilities ================================

void printIntCodeProgram(IntCodeProgram* program) {
    /*
    Nicely displays an intcode program and it's memory.
    */
    printf("[Size=%zu, Instruction Pointer=%zu, Halted=%d, Relative Base=%zu]\n", program->programSize, program->instructionPointer, program->halted, program->relativeBase);
    printf("Program: [");
    for (size_t idx = 0; idx < program->programSize; idx += 1) {
        if (idx == program->programSize - 1)
            printf("%lld", program->program[idx]);
        else
            printf("%lld, ", program->program[idx]);
    }
    printf("]\n");
    printf("Memory:  [");
    for (size_t idx = 0; idx < program->memorySize; idx += 1) {
        if (idx == program->instructionPointer) printf("->");
        if (idx == program->memorySize - 1)
            printf("%lld", program->memory[idx]);
        else
            printf("%lld, ", program->memory[idx]);
    }
    printf("]\n");

    printf("Input: ");
    printLLongArray(&program->input);

    printf("Output: ");
    printLLongArray(&program->output);
    printf("\n");
}

void initIntCodeProgramFromLLongArray(IntCodeProgram* program, LLongArray* array) {
    /*
    Initializes an intcode program from the given integer array.
    */
    program->programSize = array->numItems;
    program->program = malloc(program->programSize * sizeof(long long));
    for (size_t idx = 0; idx < array->numItems; idx += 1) program->program[idx] = array->data[idx];

    // Allocate twice as much memory as the original program takes to start. More gets allocated when
    // need as the program runs.
    program->memorySize = array->numItems * 2;
    program->memory = malloc(program->memorySize * sizeof(long long));

    program->instructionPointer = 0;
    // A never-before-run program starts off as halted, until it's run for the first time.
    program->halted = true;
    // Relative mode starts at a base of 0 until adjusted by the ADJUST_RELATIVE_BASE instruction.
    program->relativeBase = 0;

    initLLongArray(&program->input, 1024);
    initLLongArray(&program->output, 1024);

    program->inputPointer = 0;
}

void initIntCodeProgramFromFile(IntCodeProgram* program, char* inputFilePath) {
    /*
    Initializes an intcode program from a file containing the program as a comma-separated list
    of integers, like: 1,2,0,15,23,...
    */
    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    LLongArray input;
    initLLongArray(&input, 500);
    lineLen = getline(&line, &lineCap, inputFile);

    // TODO: This should technically be size_t to handle large files.
    int parserEndIdx = 0;
    while (parserEndIdx < lineLen - 1) insertLLongArray(&input, parseNumber(line, parserEndIdx, &parserEndIdx));
    fclose(inputFile);

    initIntCodeProgramFromLLongArray(program, &input);
}

void freeIntCodeProgram(IntCodeProgram* program) {
    /*
    Frees all memory associated with the program.
    */
    free(program->program);
    program->program = NULL;
    free(program->memory);
    program->memory = NULL;

    program->programSize = 0;
    program->memorySize = 0;
    program->instructionPointer = 0;

    freeLLongArray(&program->input);
    freeLLongArray(&program->output);
    program->inputPointer = 0;
}

// ================================ Running Programs ================================

int getOpcode(IntCodeProgram* program, ParameterMode* parameterModes) {
    /*
    Gets the opcode at the current instruction pointer of the program, storing the instruction's parameter
    modes in the given array at the same time.

        [1, 0, 0, 1, ->2102, ...] -> opcode=2, parameterModes=[1, 2, 0]

    The instruction pointer MUST be pointing at an opcode.
    */
    long long instruction = program->memory[program->instructionPointer];

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

int resolveNextInstruction(IntCodeProgram* program, ParameterMode* parameterModes, long long* parameters) {
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
            if (parameterModes[idx] == RELATIVE) {
                parameters[idx] = program->relativeBase + program->memory[program->instructionPointer];
            } else {
                parameters[idx] = program->memory[program->instructionPointer];
            }

        } else if (parameterModes[idx] == POSITION) {
            parameters[idx] = program->memory[program->memory[program->instructionPointer]];
        } else if (parameterModes[idx] == IMMEDIATE) {
            parameters[idx] = program->memory[program->instructionPointer];
        } else if (parameterModes[idx] == RELATIVE) {
            parameters[idx] = program->memory[program->relativeBase + program->memory[program->instructionPointer]];
        }

        program->instructionPointer += 1;
    }

    return opcode;
}

// TODO: Make a sister getFromMemory function, which returns 0 if indexing outside of the currently
// allocated memory.
void storeInMemory(IntCodeProgram* program, size_t idx, long long value) {
    /*
    Stores the value in program memory at the given idx, allocating more memory if needed.

    If the given idx is outside of the currently allocated memory, the memory space will double
    until it's sufficiently large. All new memory is initialized to 0.
    */
    if (idx >= program->memorySize) {
        size_t originalMemorySize = program->memorySize;
        // Allocate more memory, doubling it until it's greater than the necessary idx.
        while (program->memorySize <= idx) program->memorySize *= 2;
        program->memory = realloc(program->memory, program->memorySize * sizeof(long long));

        for (size_t mIdx = originalMemorySize; mIdx < program->memorySize; mIdx += 1) program->memory[mIdx] = 0;
    }

    program->memory[idx] = value;
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
        for (size_t idx = 0; idx < program->programSize; idx += 1) program->memory[idx] = program->program[idx];
        // Reset the remaining memory to 0.
        for (size_t idx = program->programSize; idx < program->memorySize; idx += 1) program->memory[idx] = 0;

        // Reset pointers and output.
        program->inputPointer = 0;
        program->output.numItems = 0;
        program->instructionPointer = 0;
        program->halted = false;
        program->relativeBase = 0;
    }

    // The opcode being processed.
    int opcode;
    // Potential opcode parameters and the modes they were evaluated in.
    ParameterMode parameterModes[MAX_OPCODE_PARAMETERS];
    long long parameters[MAX_OPCODE_PARAMETERS];
    // The instructionPointer should never be negative for a valid intcode program. This may as well be `while(true)`.
    while (program->instructionPointer >= 0) {
        // Resolve the next instruction - getting the opcode, storing the parameter values already evaluated
        // as per their mode, and advancing the instruction pointer to the start of the next instruction.
        opcode = resolveNextInstruction(program, parameterModes, parameters);

        if (opcode == ADD) {
            storeInMemory(program, parameters[2], parameters[0] + parameters[1]);
        } else if (opcode == MULTIPLY) {
            storeInMemory(program, parameters[2], parameters[0] * parameters[1]);
        } else if (opcode == STORE_INPUT) {
            if (program->inputPointer >= program->input.numItems) {
                // Waiting on input. Reset the pointer to the start of this instruction and return early. The
                // next time the program is run it will resume from this instruction, potentially with new input.
                program->instructionPointer -= 2;
                return;
            }

            storeInMemory(program, parameters[0], program->input.data[program->inputPointer]);
            program->inputPointer += 1;
        } else if (opcode == OUTPUT) {
            insertLLongArray(&program->output, parameters[0]);
        } else if (opcode == JUMP_IF_TRUE) {
            if (parameters[0] != 0) program->instructionPointer = parameters[1];
        } else if (opcode == JUMP_IF_FALSE) {
            if (parameters[0] == 0) program->instructionPointer = parameters[1];
        } else if (opcode == LESS_THAN) {
            storeInMemory(program, parameters[2], parameters[0] < parameters[1] ? 1 : 0);
        } else if (opcode == EQUALS) {
            storeInMemory(program, parameters[2], parameters[0] == parameters[1] ? 1 : 0);
        } else if (opcode == ADJUST_RELATIVE_BASE) {
            program->relativeBase += parameters[0];
        } else if (opcode == EXIT) {
            // Immediately halt the program, and mark it as halted.
            program->halted = true;
            return;
        }
    }
}

// ================================ I/O ================================

void pushInput(IntCodeProgram* program, long long input) {
    /*
    Pushes the given input to the top of the input buffer.
    */
    insertLLongArray(&program->input, input);
}

bool popOutput(IntCodeProgram* program, long long* outputDest) {
    /*
    Pops the output from the buffer (in effect removing it), storing it in the
    output destination.

    Returns false if there's no output, otherwise, returns true.
    */
    if (program->output.numItems == 0) return false;

    *outputDest = program->output.data[program->output.numItems - 1];

    program->output.numItems -= 1;
    return true;
}

void clearOutput(IntCodeProgram* program) {
    /*
    Clears the output buffer.
    */
    program->output.numItems = 0;
}
