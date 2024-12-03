#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/string.c"

#define INPUT_FILE_MAX_SIZE 100000

void problem1(char* inputFilePath) {
    /*
    The problem is asking us to sum up the products of all the `mul(n1, n2)` commands in the corrupted program.
    This reads the whole program into a buffer and then does some straightforward char-by-char parsing for the
    mult(n1, n2) symbol.

    It's a little tedious but the command is so short I figured it's not worth more complicated parsing (i.e. regex).
    */
    clock_t start = clock();

    char corruptedProgram[INPUT_FILE_MAX_SIZE + 1];

    // Read the whole file into memory.
    FILE* inputFile = fopen(inputFilePath, "r");
    size_t fileLen = fread(corruptedProgram, sizeof(char), INPUT_FILE_MAX_SIZE, inputFile);

    int idx = 0;
    int productSum = 0;

    int parserEndIdx;
    int num1, num2;
    while (idx < fileLen) {
        // Keep chugging along until we find an m.
        if (corruptedProgram[idx] != 'm') {
            idx += 1;
            continue;
        }

        // Once we found the m, check for the rest of the command, bailing if we find a character that's
        // not part of the `mult(n1, n2)` command.
        idx += 1;
        if (corruptedProgram[idx] != 'u') continue;
        idx += 1;
        if (corruptedProgram[idx] != 'l') continue;
        idx += 1;
        if (corruptedProgram[idx] != '(') continue;

        idx += 1;
        if (!isdigit(corruptedProgram[idx])) continue;
        num1 = parseNumber(corruptedProgram, idx, &parserEndIdx);

        idx = parserEndIdx;
        if (corruptedProgram[idx] != ',') continue;

        idx += 1;
        if (!isdigit(corruptedProgram[idx])) continue;
        num2 = parseNumber(corruptedProgram, idx, &parserEndIdx);

        idx = parserEndIdx;
        if (corruptedProgram[idx] != ')') continue;

        // We're good!
        productSum += num1 * num2;
        idx += 1;
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", productSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    The problem is the same as Problem 1 except there's two more commands: `do()` to enable the `mult(n1, n2)` command
    and `don't()` to disable the `mult(n1, n2)` command. WHen the `mult(n1, n2)` command is disabled, it doesn't get
    executed. The command starts off enabled.

    Same as in Problem 1, this does a straightforward char-by-char parsing. First it checks for either the `do()` or `don't()`
    commands and sets an enabled/disabled flag based on it. If disabled, the program keeps going until it reaches the next `do()`
    command, ignoring any `mult(n1, n2)` commands until it's enabled again.
    */
    clock_t start = clock();

    char corruptedProgram[INPUT_FILE_MAX_SIZE + 1];

    // Read the whole file into memory.
    FILE* inputFile = fopen(inputFilePath, "r");
    size_t fileLen = fread(corruptedProgram, sizeof(char), INPUT_FILE_MAX_SIZE, inputFile);

    int idx = 0;
    int productSum = 0;

    int parserEndIdx;
    int num1, num2;
    bool multEnabled = true;
    while (idx < fileLen) {
        // Check for the `do()` command and enable `mult(n1, n2)` if found, skipping to the next character
        // after the command if found.
        if (corruptedProgram[idx] == 'd' &&
            corruptedProgram[idx + 1] == 'o' &&
            corruptedProgram[idx + 2] == '(' &&
            corruptedProgram[idx + 3] == ')') {
            idx += 4;
            multEnabled = true;
            continue;
        }

        // Check for the `don't()` command and disable `mult(n1, n2)` if found, skipping to the next character
        // after the command if found.
        if (corruptedProgram[idx] == 'd' &&
            corruptedProgram[idx + 1] == 'o' &&
            corruptedProgram[idx + 2] == 'n' &&
            corruptedProgram[idx + 3] == '\'' &&
            corruptedProgram[idx + 4] == 't' &&
            corruptedProgram[idx + 5] == '(' &&
            corruptedProgram[idx + 6] == ')') {
            idx += 7;
            multEnabled = false;
            continue;
        }

        // If we're not at an m or mult is currently disabled continue on, looking for the next
        // m or the do command to re-enable mult.
        if (corruptedProgram[idx] != 'm' || !multEnabled) {
            idx += 1;
            continue;
        }

        idx += 1;
        if (corruptedProgram[idx] != 'u') continue;
        idx += 1;
        if (corruptedProgram[idx] != 'l') continue;
        idx += 1;
        if (corruptedProgram[idx] != '(') continue;

        idx += 1;
        if (!isdigit(corruptedProgram[idx])) continue;
        num1 = parseNumber(corruptedProgram, idx, &parserEndIdx);

        idx = parserEndIdx;
        if (corruptedProgram[idx] != ',') continue;

        idx += 1;
        if (!isdigit(corruptedProgram[idx])) continue;
        num2 = parseNumber(corruptedProgram, idx, &parserEndIdx);

        idx = parserEndIdx;
        if (corruptedProgram[idx] != ')') continue;

        // We're good! We would never get here if the command wasn't enabled.
        productSum += num1 * num2;
        idx += 1;
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", productSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
