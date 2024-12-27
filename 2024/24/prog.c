#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/map.c"
#include "../../utils/string.c"

#define AND 'A'
#define OR 'O'
#define XOR 'X'

#define MAX_INPUT_BITS 45

void problem1(char* inputFilePath) {
    /*
    Given an input of wire starting values and a set of gate operations on those wired and other, un-valued
    wires, perform the gate operations. The gates all wait for both their inputs to be present, and never
    need to be re-run - there are no loops and a gate's output won't change.

    Then, convert the values in the z wires from binary to decimal.

    Keep trying to evaluate all un-evaluated gates, skipping them if their inputs are not ready or if they've
    already been run. Store all z-values in a separate output buffer for easier conversion later.
    Once all gates have been run, convert the z-value output buffer into it's decimal equivalent.
    */
    clock_t start = clock();

    char* buff;

    // A map of wires to their known values.
    LLongMap wires;
    initLLongMap(&wires);

    // A transformation of the input, a list of a gate operations, like:
    // [inputWire1, first letter of operation, inputWire2, outputWire, ...].
    StringArray gates;
    initStringArray(&gates, 300, 3);

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    // Parse the first part of the input into the wire mapping.
    int parserEndIdx;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 1) {
        buff = malloc(4);
        memcpy(buff, &line[0], 3);
        buff[3] = '\0';

        setLLongMap(&wires, buff, parseNumber(line, 5, &parserEndIdx));
    }

    // Parse the logic gates
    int postOperationOffset = 0;
    int maxZWire = -1, zWireNumber;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        // First input wire
        buff = malloc(4);
        memcpy(buff, &line[0], 3);
        buff[3] = '\0';
        insertStringArray(&gates, buff);

        // Operation (first character).
        buff = malloc(2);
        memcpy(buff, &line[4], 1);
        buff[3] = '\0';
        insertStringArray(&gates, buff);
        // This just lessens the following indices by 1, since 'OR' has one less character than 'AND' and 'XOR'.
        postOperationOffset = line[4] == 'O' ? -1 : 0;

        // Second input wire
        buff = malloc(4);
        memcpy(buff, &line[8 + postOperationOffset], 3);
        buff[3] = '\0';
        insertStringArray(&gates, buff);

        // Output wire
        buff = malloc(4);
        memcpy(buff, &line[15 + postOperationOffset], 3);
        buff[3] = '\0';
        insertStringArray(&gates, buff);

        // Figure out the max z wire number, to correctly size an z-value output buffer below.
        if (buff[0] == 'z') {
            zWireNumber = parseNumber(buff, 0, &parserEndIdx);
            maxZWire = zWireNumber > maxZWire ? zWireNumber : maxZWire;
        }
    }

    fclose(inputFile);

    // Stores the values of the z wire (indexed by the wire name, z01 = outputBuffer[1]).
    int outputBuffer[maxZWire + 1];

    // Keep looping through the operations until all of them have already been performed (i.e., until
    // we don't perform any operations on a loop-through).
    char *wire1, *operation, *wire2, *outputWire;
    long long input1, input2, output;
    bool operationPerformed = true;
    while (operationPerformed) {
        operationPerformed = false;

        // Attempt each gate operation.
        for (int idx = 0; idx < gates.numItems; idx += 4) {
            wire1 = gates.data[idx];
            operation = gates.data[idx + 1];
            wire2 = gates.data[idx + 2];
            outputWire = gates.data[idx + 3];

            // If we've already got the output value, no need to recompute.
            if (getLLongMap(&wires, outputWire, &output)) continue;

            // If we don't have the input values yet, skip for now.
            if (!(getLLongMap(&wires, wire1, &input1) && getLLongMap(&wires, wire2, &input2))) continue;

            // Otherwise, we've performed at least one operation this cycle.
            operationPerformed = true;

            if (operation[0] == AND) {
                output = input1 & input2;
            } else if (operation[0] == OR) {
                output = input1 | input2;
            } else if (operation[0] == XOR) {
                output = input1 ^ input2;
            }

            // Store the z wire value in the output buffer.
            if (outputWire[0] == 'z') outputBuffer[parseNumber(outputWire, 1, &parserEndIdx)] = output;

            // Store the wire value.
            setLLongMap(&wires, outputWire, output);
        }
    }

    // Convert the binary representation to it's decimal value.
    long long result = 0;
    for (int idx = maxZWire; idx >= 0; idx -= 1) result = result * 2 + outputBuffer[idx];

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", result, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    The circuit is a ripple-carry adder, adding the numbers represented by the x and y input
    wires together, outputted in z. But it's broken! There's 4 pairs of wires that need to
    be swapped for the circuit to work.

    This is not a solution but rather a testing ground for manually debugging the circuit.
    Ignoring the input xs and ys, you can provide your own (in the `x` and `y` variables), and it'll
    convert that to the input wires.

    From there, the program runs the circuit (as in part 1), and outputs the z as both a decimal
    and binary.

    Laying the binary of the x, y, and z wires on top of each other, you can manually perform the
    binary addition to see where things go wrong.

    My process was to try an addition, seeing the first bit it went wrong at, and backtracking from
    the z wire representing that bit until I found something was wrong in the wiring. I had a few
    false positives where the circuit worked for some numbers but not others.
    */
    char* buff;

    // A map of wires to their known values.
    LLongMap wires;
    initLLongMap(&wires);

    // A transformation of the input, a list of a gate operation, like:
    // [inputWire1, first letter of operation, inputWire2, outputWire, ...].
    StringArray gates;
    initStringArray(&gates, 1000, 4);

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Ignore the actual input values, but still get the number of input wires needed
    // for when the inputs are replicated below.
    int maxInputWire = -1, inputWire = 0, xIdx = 0, yIdx = 0;
    int xBuffer[MAX_INPUT_BITS] = {0};
    int yBuffer[MAX_INPUT_BITS] = {0};
    int parserEndIdx;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 1) {
        buff = malloc(4);
        memcpy(buff, &line[0], 3);
        buff[3] = '\0';

        inputWire = parseNumber(buff, 0, &parserEndIdx);
        if (inputWire > maxInputWire) maxInputWire = inputWire;
    }

    // Update these to test different additions.
    long long x = 1111111111;
    long long y = 1111111111;

    // Convert x and y into binary, storing it in the buffers and the wire map.
    xIdx = 0, yIdx = 0;
    while (x > 0) {
        xBuffer[xIdx] = x % 2;

        buff = malloc(4);
        sprintf(buff, "x%02d", xIdx);
        buff[3] = '\0';
        setLLongMap(&wires, buff, x % 2);

        x = x / 2;
        xIdx += 1;
    }
    while (y > 0) {
        yBuffer[yIdx] = y % 2;

        buff = malloc(4);
        sprintf(buff, "y%02d", yIdx);
        buff[3] = '\0';
        setLLongMap(&wires, buff, y % 2);

        y = y / 2;
        yIdx += 1;
    }

    // Fill any remaining bits with 0's, so the circuit can be run.
    long long throwaway;  // Throwaway since `getLLongMap` needs a pointer.
    int idx = 0;
    while (idx < maxInputWire) {
        buff = malloc(4);
        sprintf(buff, "x%02d", idx);
        buff[3] = '\0';
        if (!getLLongMap(&wires, buff, &throwaway)) setLLongMap(&wires, buff, 0);

        buff = malloc(4);
        sprintf(buff, "y%02d", idx);
        buff[3] = '\0';
        if (!getLLongMap(&wires, buff, &throwaway)) setLLongMap(&wires, buff, 0);

        idx += 1;
    }

    // Parse the logic gates, same as part 1.
    int postOperationOffset = 0;
    int maxZWire = -1, zWireNumber;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        // First input wire
        buff = malloc(4);
        memcpy(buff, &line[0], 3);
        buff[3] = '\0';
        insertStringArray(&gates, buff);

        // Operation (first character).
        buff = malloc(2);
        memcpy(buff, &line[4], 1);
        buff[3] = '\0';
        insertStringArray(&gates, buff);
        postOperationOffset = line[4] == 'O' ? -1 : 0;

        // Second input wire
        buff = malloc(4);
        memcpy(buff, &line[8 + postOperationOffset], 3);
        buff[3] = '\0';
        insertStringArray(&gates, buff);

        // Output wire
        buff = malloc(4);
        memcpy(buff, &line[15 + postOperationOffset], 3);
        buff[3] = '\0';
        insertStringArray(&gates, buff);

        if (buff[0] == 'z') {
            zWireNumber = parseNumber(buff, 0, &parserEndIdx);
            maxZWire = zWireNumber > maxZWire ? zWireNumber : maxZWire;
        }
    }

    fclose(inputFile);

    // Stores the values of the z wire (indexed by the wire name, z01 = outputBuffer[1]).
    int outputBuffer[maxZWire + 1];

    // Run the circuit 9as in part 1).
    char *wire1, *operation, *wire2, *outputWire;
    long long input1, input2, output;
    bool operationPerformed = true;
    while (operationPerformed) {
        operationPerformed = false;

        // Attempt each gate operation.
        for (int idx = 0; idx < gates.numItems; idx += 4) {
            wire1 = gates.data[idx];
            operation = gates.data[idx + 1];
            wire2 = gates.data[idx + 2];
            outputWire = gates.data[idx + 3];

            // If we've already got the output value, no need to recompute.
            if (getLLongMap(&wires, outputWire, &output)) continue;

            // If we don't have the input values yet, skip for now.
            if (!(getLLongMap(&wires, wire1, &input1) && getLLongMap(&wires, wire2, &input2))) continue;

            // Otherwise, we've performed at least one operation this cycle.
            operationPerformed = true;

            if (operation[0] == AND) {
                output = input1 & input2;
            } else if (operation[0] == OR) {
                output = input1 | input2;
            } else if (operation[0] == XOR) {
                output = input1 ^ input2;
            }

            // Store the z wire value in the output buffer.
            if (outputWire[0] == 'z') {
                outputBuffer[parseNumber(outputWire, 1, &parserEndIdx)] = output;
            }

            // Store the wire value so we don't attempt to recompute it.
            setLLongMap(&wires, outputWire, output);
        }
    }

    // Everything below is for debugging, overlaying the x, y, and z bits, alongside the
    // expected and actual decimal additions.

    // Reconstruct the original, decimal x and y values, for display.
    long long xInput = 0, yInput = 0;
    for (int idx = maxInputWire; idx >= 0; idx -= 1) {
        xInput = xInput * 2 + xBuffer[idx];
        yInput = yInput * 2 + yBuffer[idx];
    }

    printf("X: 0 ");
    for (int idx = maxInputWire; idx >= 0; idx -= 1) {
        printf("%d ", xBuffer[idx]);
    }
    printf("\n");
    printf("Y: 0 ");
    for (int idx = maxInputWire; idx >= 0; idx -= 1) {
        printf("%d ", yBuffer[idx]);
    }
    printf("\n");
    printf("Z: ");
    for (int idx = maxZWire; idx >= 0; idx -= 1) {
        printf("%d ", outputBuffer[idx]);
    }
    printf("\n");

    // Convert the binary array to it's decimal value.
    long long result = 0;
    for (int idx = maxZWire; idx >= 0; idx -= 1) result = result * 2 + outputBuffer[idx];

    printf("%lld + %lld = %lld (Actual: %lld)\n", xInput, yInput, result, xInput + yInput);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
