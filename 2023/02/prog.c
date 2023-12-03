#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../../utils/string.c"


int max(int a, int b) {
    return (a > b) ? a : b;
}


void problem_1_and_2() {
    clock_t start = clock();

    int RED_TARGET = 12, GREEN_TARGET = 13, BLUE_TARGET = 14;

    // Each game starts with "Game <ID>:", so to get the ID of the game we have to start at the 5th index.
    int GAME_ID_STARTING_IDX = 5;

    FILE* gameFile = fopen("2023/02/input.txt", "r");

    int gameIdSum = 0, cubePowerSum = 0;

    char *line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int gameIdEndIdx = 0;
    while ((lineLen = getline(&line, &lineCap, gameFile)) > 0) {
        int maxRed = -1, maxBlue = -1, maxGreen = -1;

        // get the id of the game.
        char* colon = strchr(line, ':');
        gameIdEndIdx = (int)(colon - line);
        char gameId[gameIdEndIdx - GAME_ID_STARTING_IDX + 1];
        substring(line, gameId, GAME_ID_STARTING_IDX, gameIdEndIdx);

        // The first number starts two spaces right of the colon.
        int idx = gameIdEndIdx + 2;
        while (idx < lineLen) {
            char* numberSpace = strchr(line + idx, ' ');
            int numberSpaceIdx = (int)(numberSpace - line);

            char numberString[numberSpaceIdx - idx + 1];
            substring(line, numberString, idx, numberSpaceIdx);
            idx = numberSpaceIdx + 1;

            char color = line[idx];
            if (color == 'r') {
                maxRed = max(maxRed, atoi(numberString));
                idx += 5;
            } else if (color == 'g') {
                maxGreen = max(maxGreen, atoi(numberString));
                idx += 7;
            } else {
                // must be blue
                maxBlue = max(maxBlue, atoi(numberString));
                idx += 6;
            }
        }

        if (maxRed <= RED_TARGET && maxGreen <= GREEN_TARGET && maxBlue <= BLUE_TARGET) {
            gameIdSum += atoi(gameId);
        }

        cubePowerSum += (maxRed * maxGreen * maxBlue);
    }

    clock_t end = clock();
    double timeElapsedInSeconds = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Problem 01: %d [%fs]\n", gameIdSum, timeElapsedInSeconds);
    printf("Problem 02: %d [%fs]\n", cubePowerSum, timeElapsedInSeconds);
}

int main() {
    problem_1_and_2();

    return 0;
}