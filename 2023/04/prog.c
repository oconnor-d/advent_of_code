#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../../utils/array.c"
#include "../../utils/string.c"


void problem1And2() {
    clock_t start = clock();

    FILE* inputFile = fopen("2023/04/input.txt", "r");

    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLength;

    IntArray winningNumbers, givenNumbers, matchingNumbersPerCard;
    initIntArray(&matchingNumbersPerCard, 64);
    while ((lineLength = getline(&line, &lineCap, inputFile)) > 0) {
        initIntArray(&winningNumbers, 10);
        initIntArray(&givenNumbers, 10);

        // Jump straight past the "Card   1:" part of each line.
        char* cardNumbers = strchr(line, ':');

        int numberStartIdx, numberEndIdx;
        while (*cardNumbers != '|') {
            insertIntArray(&winningNumbers, parseFirstNumber(cardNumbers, &numberStartIdx, &numberEndIdx));
            cardNumbers += numberEndIdx + 1;
        }

        while (*cardNumbers) {
            insertIntArray(&givenNumbers, parseFirstNumber(cardNumbers, &numberStartIdx, &numberEndIdx));
            cardNumbers += numberEndIdx + 1;
        }

        int matches = 0;
        for (int idx = 0; idx < givenNumbers.numItems; idx += 1) {
            if (containsIntArray(&winningNumbers, givenNumbers.data[idx])) {
                matches += 1;
            }

            cardNumbers += numberEndIdx + 1;
        }

        insertIntArray(&matchingNumbersPerCard, matches);

        freeIntArray(&winningNumbers);
        freeIntArray(&givenNumbers);
    }

    int cardTotals = 0;
    for (int idx = 0; idx < matchingNumbersPerCard.numItems; idx += 1) {
        if (matchingNumbersPerCard.data[idx] > 0) {
            // (1 << x) == 2^x. We subtract one since the first match is the 0 to 1 jump.
            cardTotals += (1 << (matchingNumbersPerCard.data[idx] - 1));
        }
    }

    clock_t end = clock();
    printf("Problem 12: %d [%.2fms]\n", cardTotals, (double)(end - start) / CLOCKS_PER_SEC * 1000);

    // Holds how many copies of each card we have, starting with 1 of each.
    int copiedCards[matchingNumbersPerCard.numItems];
    for (int idx = 0; idx < matchingNumbersPerCard.numItems; idx += 1) {
        copiedCards[idx] = 1;
    }

    // For each card, add the number of copies of following cards based off the previously counted
    // matching numbers for that card.
    for (int idx = 0; idx < matchingNumbersPerCard.numItems; idx += 1) {
        int matches = matchingNumbersPerCard.data[idx];
        int copies = copiedCards[idx];

        while (matches > 0) {
            copiedCards[idx + matches] += copies;
            matches -= 1;
        }
    }

    int numScratchCards = 0;
    for (int idx = 0; idx < matchingNumbersPerCard.numItems; idx += 1) {
        numScratchCards += copiedCards[idx];
    }

    freeIntArray(&matchingNumbersPerCard);

    end = clock();
    printf("Problem 02: %d [%.2fms]\n", numScratchCards, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}


int main() {
    problem1And2();

    return 0;
}