#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/intcode.c"

void problem1(char* inputFilePath) {
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    /*
    Springdroids have a sensor that can detect whether there is ground at various distances in the direction
    it is facing; these values are provided in read-only registers. Your springdroid can detect ground at four
    distances:
        one tile away (A)
        two tiles away (B)
        three tiles away (C)
        and four tiles away (D)

    If there is ground at the given distance, the register will be true; if there is a hole, the register will be false.

    There are only three instructions available in springscript:

    AND X Y sets Y to true if both X and Y are true; otherwise, it sets Y to false.
    OR X Y sets Y to true if at least one of X or Y is true; otherwise, it sets Y to false.
    NOT X Y sets Y to true if X is false; otherwise, it sets Y to false.
    */

    /*
    JUMP
        @
        ##.##.##


    */

    // GROUND = TRUE, HOLE = FALSE

    // Store if 2 is hole in T
    pushInputASCII(&program, "NOT B T");
    // Store if 3 is hole in J.
    pushInputASCII(&program, "NOT C J");
    // Store if 2 OR 3 is hole in J
    pushInputASCII(&program, "OR T J");
    // And if 4 is safe, jump
    pushInputASCII(&program, "AND D J");
    // else if there's an empty space right in front of you, jump (base case).
    pushInputASCII(&program, "NOT A T");
    pushInputASCII(&program, "OR T J");

    pushInputASCII(&program, "WALK");

    intcodeRun(&program);

    // Print the feed, minus the final output which is the non-ASCII answer.
    for (int idx = 0; idx < program.output.numItems - 1; idx += 1) {
        printf("%c", (char)program.output.data[idx]);
    }
    printf("\n");

    long long hullDamage;
    popOutput(&program, &hullDamage);

    clock_t end = clock();
    printf("Problem 01: %lld [%.2fms]\n", hullDamage, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    /*
    Springdroids have a sensor that can detect whether there is ground at various distances in the direction
    it is facing; these values are provided in read-only registers. Your springdroid can detect ground at four
    distances:
        one tile away (A)
        two tiles away (B)
        three tiles away (C)
        and four tiles away (D)
        Register E indicates whether there is ground five tiles away.
        Register F indicates whether there is ground six tiles away.
        Register G indicates whether there is ground seven tiles away.
        Register H indicates whether there is ground eight tiles away.
        Register I indicates whether there is ground nine tiles away.

    If there is ground at the given distance, the register will be true; if there is a hole, the register will be false.

    There are only three instructions available in springscript:

    AND X Y sets Y to true if both X and Y are true; otherwise, it sets Y to false.
    OR X Y sets Y to true if at least one of X or Y is true; otherwise, it sets Y to false.
    NOT X Y sets Y to true if X is false; otherwise, it sets Y to false.
    */

    /*
    JUMP
        @
        ##.##.##


    */

    // GROUND = TRUE, HOLE = FALSE

    // Store if 2 is hole in T
    pushInputASCII(&program, "NOT B T");
    // Store if 3 is hole in J.
    pushInputASCII(&program, "NOT C J");
    // Store if 2 OR 3 is hole in J
    pushInputASCII(&program, "OR T J");
    // And if 4 is safe, jump
    pushInputASCII(&program, "AND D J");
    // AND if 8 is also safe, jump. If 8 is not safe, we'll have to jump a little later, because if not
    // an immediate jump will kill the robot.
    pushInputASCII(&program, "AND H J");
    // else if there's an empty space right in front of you, jump (base case).
    pushInputASCII(&program, "NOT A T");
    pushInputASCII(&program, "OR T J");

    pushInputASCII(&program, "RUN");

    intcodeRun(&program);

    // Print the feed, minus the final output which is the non-ASCII answer.
    for (int idx = 0; idx < program.output.numItems - 1; idx += 1) {
        printf("%c", (char)program.output.data[idx]);
    }
    printf("\n");

    long long hullDamage;
    popOutput(&program, &hullDamage);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", hullDamage, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
