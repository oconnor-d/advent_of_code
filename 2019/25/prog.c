#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/intcode.c"

void problem1(char* inputFilePath) {
    /*
    It's a text adventure game! I played it manually.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    char input;
    while (true) {
        // Run the program.
        intcodeRun(&program);

        // Print the output.
        for (int idx = 0; idx < program.output.numItems; idx += 1) {
            printf("%c", (char)program.output.data[idx]);
        }
        printf("\n");

        // If the program's halted, it's over.
        if (program.halted) break;

        // Input command.
        while (input != '\n') {
            scanf("%c", &input);
            pushInput(&program, (int)input);
        }

        // Reset I/O.
        input = '\0';
        clearOutput(&program);
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", 0, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);

    return 0;
}
