#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

#define INPUT_PROGRAM_SIZE 50

#define A_OPERAND 4
#define B_OPERAND 5
#define C_OPERAND 6

#define ADV 0
#define BXL 1
#define BST 2
#define JNZ 3
#define BXC 4
#define OUT 5
#define BDV 6
#define CDV 7

long long getComboOperandValue(long long operand, long long A, long long B, long long C) {
    /*
    Gets the value of a combo-operand, 1-3 for 1-3, and then the registers for 4-6. 7 is
    also returned as it's literal value, but it's not a valid combo operator in a valid
    program.
    */
    if (operand == A_OPERAND) return A;
    if (operand == B_OPERAND) return B;
    if (operand == C_OPERAND) return C;

    return operand;
}

long long power(long long x, long long n) {
    /*
    Get's x to the power of n.
    */
    if (n == 0) return 1;

    for (long long idx = 1; idx < n; idx += 1) {
        x *= 2;
    }

    return x;
}

void problem1(char* inputFilePath) {
    /*
    We're given three registers, A, B, and C, and a program to run with a list of int-based
    opcodes with various definitions.

    The program is a list of: [OP_CODE1, OPERAND1, ...], where the op code uses the operand and the
    registers to perform some computation.

    Some operands are the literal values, and some represent a combo operand, either being literal values
    or references to read from the registers.

    This is pretty straightforward, implement all the opcodes and go through the program using an instruction
    pointer.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int parserEndIdx;

    // Read the starting register values.
    int A, B, C;
    getline(&line, &lineCap, inputFile);
    A = parseNumber(line, 0, &parserEndIdx);
    getline(&line, &lineCap, inputFile);
    B = parseNumber(line, 0, &parserEndIdx);
    getline(&line, &lineCap, inputFile);
    C = parseNumber(line, 0, &parserEndIdx);

    // Throw away the newline.
    getline(&line, &lineCap, inputFile);

    // Load the program into `line`.
    lineLen = getline(&line, &lineCap, inputFile);

    // Load the program into an array.
    IntArray program;
    initIntArray(&program, INPUT_PROGRAM_SIZE);

    parserEndIdx = 0;
    while (parserEndIdx < lineLen - 1) {
        insertIntArray(&program, parseNumber(line, parserEndIdx, &parserEndIdx));
    }

    fclose(inputFile);

    // The output the program "prints" gets put into this output buffer, and printed (for real) after
    // the program terminates.
    IntArray outputBuffer;
    initIntArray(&outputBuffer, 100);

    // Run the program until the instruction pointer goes out of bounds.
    int instruction, operand, instructionPointer = 0;
    while (instructionPointer >= 0 && instructionPointer < program.numItems) {
        instruction = program.data[instructionPointer];
        operand = program.data[instructionPointer + 1];

        if (instruction == ADV) {
            A = A / power(2, getComboOperandValue(operand, A, B, C));
        } else if (instruction == BXL) {
            B = B ^ operand;
        } else if (instruction == BST) {
            B = getComboOperandValue(operand, A, B, C) % 8;
        } else if (instruction == JNZ) {
            if (A != 0) {
                instructionPointer = operand;
                continue;
            }
        } else if (instruction == BXC) {
            B = B ^ C;
        } else if (instruction == OUT) {
            insertIntArray(&outputBuffer, getComboOperandValue(operand, A, B, C) % 8);
        } else if (instruction == BDV) {
            B = A / power(2, getComboOperandValue(operand, A, B, C));
        } else if (instruction == CDV) {
            C = A / power(2, getComboOperandValue(operand, A, B, C));
        }

        instructionPointer += 2;
    }

    printf("Problem 01: ");
    // Print the output buffer
    for (int idx = 0; idx < outputBuffer.numItems; idx += 1) {
        // Exclude the comma for the last bit
        if (idx == outputBuffer.numItems - 1)
            printf("%d", outputBuffer.data[idx]);
        else
            printf("%d,", outputBuffer.data[idx]);
    }

    clock_t end = clock();
    printf(" [%.2fms]\n", (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Ignore the starting A register value from the input, and find the starting A register value that would
    cause the program to output itself.

    In other words, look for an A that turns the program into a quine: https://en.wikipedia.org/wiki/Quine_(computing).

    I got the key insight that informs this program from reddit, which is really the hard part of this problem, what
    I have here is a pretty straightforward implementation of that idea.

    Basically, all the % 8's are the give-away. We only ever output the last 3 bits of a number. So, we build the program
    in reverse. First we find the starting A value that gets us the last letter of the program, and use that value
    times 8 (which preserves the valid last three bits) as the value that gets us the last two letters of the program,
    and so on until we've found all the required numbers.

    This massively cuts down the amount of numbers we need to check.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int parserEndIdx;

    // Read the starting register values.
    long long A, B, C, bStart, cStart;
    getline(&line, &lineCap, inputFile);
    A = parseNumber(line, 0, &parserEndIdx);
    getline(&line, &lineCap, inputFile);
    bStart = parseNumber(line, 0, &parserEndIdx);
    getline(&line, &lineCap, inputFile);
    cStart = parseNumber(line, 0, &parserEndIdx);

    // Throw away the newline.
    getline(&line, &lineCap, inputFile);

    // Load the program into `line`
    lineLen = getline(&line, &lineCap, inputFile);

    // Load the program into an array.
    IntArray program;
    initIntArray(&program, INPUT_PROGRAM_SIZE);

    parserEndIdx = 0;
    while (parserEndIdx < lineLen - 1) {
        insertIntArray(&program, parseNumber(line, parserEndIdx, &parserEndIdx));
    }

    fclose(inputFile);

    // Allocate space for the output buffer of the program, and the target sub-program to check the
    // output against.
    IntArray outputBuffer, targetBuffer;
    initIntArray(&outputBuffer, 100);
    initIntArray(&targetBuffer, 100);

    // Start at 0.
    long long aStart = 0;
    bool quineFound = false;

    // For each sub-program (starting from the end, like [0] -> [3, 0] -> [1, 3, 0] for [1, 3, 0]), run
    // the program with the starting A value that satisfied the previous sub-list, multiplied by 8.
    for (int idx = program.numItems - 1; idx >= 0; idx -= 1) {
        outputBuffer.numItems = 0;
        targetBuffer.numItems = 0;

        // Create the sub-list
        for (int subIdx = idx; subIdx < program.numItems; subIdx += 1) insertIntArray(&targetBuffer, program.data[subIdx]);

        // Until we find an a value that matches the sub-program, keep going, incrementing the starting value
        // of A by one.
        while (true) {
            quineFound = false;

            // Reset the registers, trying the new starting value for A.
            A = aStart, B = bStart, C = cStart;

            // Run the program until the instruction pointer goes out of bounds.
            int instruction, operand, instructionPointer = 0;
            while (instructionPointer >= 0 && instructionPointer < program.numItems) {
                instruction = program.data[instructionPointer];
                operand = program.data[instructionPointer + 1];

                if (instruction == ADV) {
                    A = A / power(2, getComboOperandValue(operand, A, B, C));
                } else if (instruction == BXL) {
                    B = B ^ operand;
                } else if (instruction == BST) {
                    B = getComboOperandValue(operand, A, B, C) % 8;
                } else if (instruction == JNZ) {
                    if (A != 0) {
                        instructionPointer = operand;
                        continue;
                    }
                } else if (instruction == BXC) {
                    B = B ^ C;
                } else if (instruction == OUT) {
                    insertIntArray(&outputBuffer, getComboOperandValue(operand, A, B, C) % 8);
                    // Stop running the program once we've hit the desired length.
                    if (outputBuffer.numItems == targetBuffer.numItems) break;
                } else if (instruction == BDV) {
                    B = A / power(2, getComboOperandValue(operand, A, B, C));
                } else if (instruction == CDV) {
                    C = A / power(2, getComboOperandValue(operand, A, B, C));
                }

                instructionPointer += 2;
            }

            // Check if the program output is the same as the sub-program.
            if (outputBuffer.numItems == targetBuffer.numItems) {
                quineFound = true;
                for (int x = 0; x < targetBuffer.numItems; x += 1) {
                    if (outputBuffer.data[x] != targetBuffer.data[x]) {
                        quineFound = false;
                        break;
                    }
                }
            }

            // If it's not, increment A and go again.
            if (!quineFound) {
                aStart += 1;
                outputBuffer.numItems = 0;
            } else {
                // Otherwise, we've found it, and can start at the next sub-list at 8 times the value
                // of the start value of A for this time around.
                aStart *= 8;
                break;
            }
        }
    }

    // Undo that last multiplication after the full program has been reproduced.
    aStart = aStart / 8;

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", aStart, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
