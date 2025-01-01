#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../../utils/intcode.c"

#define MAX_X 500
#define MAX_Y 500

#define EMPTY 0
#define WALL 1
#define BLOCK 2
#define PADDLE 3
#define BALL 4

// If true, animates the entire game to the console.
#define DISPLAY_GAME false

void problem1(char* inputFilePath) {
    /*
    Run the first frame of the game and counts the number of tile blocks.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    intcodeRun(&program);

    int blockTiles = 0;
    for (int idx = 0; idx < program.output.numItems; idx += 3) {
        if (program.output.data[idx + 2] == BLOCK) blockTiles += 1;
    }

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", blockTiles, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Runs the game, moving the paddle left if the ball is to the left, right if it's to the right,
    and keeping it neutral if the paddle is right below the ball.
    */
    clock_t start = clock();

    IntCodeProgram program;
    initIntCodeProgramFromFile(&program, inputFilePath);

    long long score = 0;

    // The program only outputs the full game state on the first run, after that it only
    // outputs updated tiles. On the first run, load the full state and note the width and
    // height of the grid.
    bool firstRun = true;
    int maxX = 0,
        maxY = 0;

    // For printing the game state to the terminal.
    char outputBuffer[MAX_Y][MAX_X];

    // Insert unlimited quarters
    program.program[0] = 2;

    // Run the program until it halts, moving the paddle as needed.
    do {
        intcodeRun(&program);

        // Get the ball and paddle coordinates from the output, and store the state in
        // the output buffer for display.
        int x, y, ballX, paddleX, tileId;
        for (int idx = 0; idx < program.output.numItems; idx += 3) {
            x = program.output.data[idx];
            y = program.output.data[idx + 1];
            tileId = program.output.data[idx + 2];

            // The score is preceeded by an x of -1 and y of 0, to differentiate it from the
            // tile output sequences.
            if (x == -1 && y == 0) {
                score = tileId;
                continue;
            }

            // Store the max coordinates for displaying the game.
            if (firstRun && x > maxX) maxX = x;
            if (firstRun && y > maxY) maxY = y;

            /*
            0 is an empty tile. No game object appears in this tile.
            1 is a wall tile. Walls are indestructible barriers.
            2 is a block tile. Blocks can be broken by the ball.
            3 is a horizontal paddle tile. The paddle is indestructible.
            4 is a ball tile. The ball moves diagonally and bounces off objects.
            */
            if (tileId == EMPTY)
                outputBuffer[y][x] = ' ';
            else if (tileId == WALL)
                outputBuffer[y][x] = '=';
            else if (tileId == BLOCK)
                outputBuffer[y][x] = '#';
            else if (tileId == PADDLE) {
                outputBuffer[y][x] = '-';
                paddleX = x;
            } else if (tileId == BALL) {
                outputBuffer[y][x] = 'o';
                ballX = x;
            }
        }

        // Move the paddle towards the ball.
        if (ballX > paddleX)
            pushInput(&program, 1);
        else if (ballX < paddleX)
            pushInput(&program, -1);
        else
            pushInput(&program, 0);

        if (DISPLAY_GAME) {
            // Display the game.

            // This clears the console and resets the cursor to (0,0). This probably isn't very
            // portable but who cares this is for me.
            printf("%c[2J%c[;H", (char)27, (char)27);
            for (y = 0; y < maxY + 1; y += 1) {
                for (x = 0; x < maxX + 1; x += 1) {
                    printf("%c", outputBuffer[y][x]);
                }
                printf("\n");
            }
            for (x = 0; x < maxX + 1; x += 1) printf("=");
            printf("\nScore: %lld\n", score);
            usleep(7500);
        }

        // Clear the output buffer.
        clearOutput(&program);

        firstRun = false;
    } while (!program.halted);
    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", score, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
