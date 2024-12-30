#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/intcode.c"

void problem1(char* inputFilePath) {
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    long long maxSignal = -1;

    // Try all valid combinations of input signals (0-4).
    for (int a = 0; a <= 4; a += 1) {
        for (int b = 0; b <= 4; b += 1) {
            // These continues are to ensure that no two numbers in the phase settings are the same.
            if (b == a) continue;

            for (int c = 0; c <= 4; c += 1) {
                if (c == a || c == b) continue;

                for (int d = 0; d <= 4; d += 1) {
                    if (d == a || d == b || d == c) continue;

                    for (int e = 0; e <= 4; e += 1) {
                        if (e == a || e == b || e == c || e == d) continue;

                        // Now that we've got a valid signal sequence, run them and keep track
                        // of the maximum output signal.

                        // Amp A
                        program.input.numItems = 0;
                        insertLLongArray(&program.input, a);
                        insertLLongArray(&program.input, 0);
                        intcodeRun(&program);

                        // Amp B
                        program.input.numItems = 0;
                        insertLLongArray(&program.input, b);
                        insertLLongArray(&program.input, program.output.data[0]);
                        intcodeRun(&program);

                        // Amp C
                        program.input.numItems = 0;
                        insertLLongArray(&program.input, c);
                        insertLLongArray(&program.input, program.output.data[0]);
                        intcodeRun(&program);

                        // Amp D
                        program.input.numItems = 0;
                        insertLLongArray(&program.input, d);
                        insertLLongArray(&program.input, program.output.data[0]);
                        intcodeRun(&program);

                        // Amp E
                        program.input.numItems = 0;
                        insertLLongArray(&program.input, e);
                        insertLLongArray(&program.input, program.output.data[0]);
                        intcodeRun(&program);

                        if (program.output.data[0] > maxSignal) maxSignal = program.output.data[0];
                    }
                }
            }
        }
    }

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", maxSignal, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Like part 1, but now the programs keep running in a loop until amp E halts, with E's output feeding back into
    A when E doesn't halt (i.e., it's waiting for another input).

    For this part, the Intcode interpreter was updated to handle waiting for inputs. If a program hits the STORE_INPUT
    command without any new inputs in the buffer, it returns early. The next time the program runs, it picks up from
    where it left off. I also added a flag to tell if a program completed by hitting the EXIT opcode or not, which is
    used to tell when the E program is done for good.

    Besides that, the solution is pretty similar to part 1, the only difference is the program running loops until
    E is halted.
    */
    clock_t start = clock();

    IntCodeProgram ampAProgram, ampBProgram, ampCProgram, ampDProgram, ampEProgram;
    initIntCodeProgramFromFile(&ampAProgram, inputFilePath);
    initIntCodeProgramFromFile(&ampBProgram, inputFilePath);
    initIntCodeProgramFromFile(&ampCProgram, inputFilePath);
    initIntCodeProgramFromFile(&ampDProgram, inputFilePath);
    initIntCodeProgramFromFile(&ampEProgram, inputFilePath);

    long long maxSignal = -1;

    long long previousSignal;
    // Try all valid combinations of input signals (5-9).
    for (int a = 5; a <= 9; a += 1) {
        for (int b = 5; b <= 9; b += 1) {
            // These continues are to ensure that no two numbers in the phase settings are the same.
            if (b == a) continue;

            for (int c = 5; c <= 9; c += 1) {
                if (c == a || c == b) continue;

                for (int d = 5; d <= 9; d += 1) {
                    if (d == a || d == b || d == c) continue;

                    for (int e = 5; e <= 9; e += 1) {
                        if (e == a || e == b || e == c || e == d) continue;

                        // Initialize the phase signals for each amp.
                        ampAProgram.input.numItems = 0;
                        insertLLongArray(&ampAProgram.input, a);

                        ampBProgram.input.numItems = 0;
                        insertLLongArray(&ampBProgram.input, b);

                        ampCProgram.input.numItems = 0;
                        insertLLongArray(&ampCProgram.input, c);

                        ampDProgram.input.numItems = 0;
                        insertLLongArray(&ampDProgram.input, d);

                        ampEProgram.input.numItems = 0;
                        insertLLongArray(&ampEProgram.input, e);

                        // Until the Amp E program halts, keep running the array of amp programs, feeding the output
                        // of the previous amp into the next one in the chain, with E looping back into A.

                        // As per the problem, Amp A first starts with a signal of 0 to kick things off.
                        previousSignal = 0;
                        do {
                            insertLLongArray(&ampAProgram.input, previousSignal);
                            intcodeRun(&ampAProgram);
                            previousSignal = ampAProgram.output.data[ampAProgram.output.numItems - 1];

                            insertLLongArray(&ampBProgram.input, previousSignal);
                            intcodeRun(&ampBProgram);
                            previousSignal = ampBProgram.output.data[ampBProgram.output.numItems - 1];

                            insertLLongArray(&ampCProgram.input, previousSignal);
                            intcodeRun(&ampCProgram);
                            previousSignal = ampCProgram.output.data[ampCProgram.output.numItems - 1];

                            insertLLongArray(&ampDProgram.input, previousSignal);
                            intcodeRun(&ampDProgram);
                            previousSignal = ampDProgram.output.data[ampDProgram.output.numItems - 1];

                            insertLLongArray(&ampEProgram.input, previousSignal);
                            intcodeRun(&ampEProgram);
                            previousSignal = ampEProgram.output.data[ampEProgram.output.numItems - 1];
                        } while (!ampEProgram.halted);

                        // At this point, previousSignal is the most recent output from Amp E's program.
                        if (previousSignal > maxSignal) maxSignal = previousSignal;
                    }
                }
            }
        }
    }

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", maxSignal, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
