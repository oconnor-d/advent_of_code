#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/map.c"
#include "../../utils/string.c"

long long designsPossible(char* design, int designLength, StringArray* availableStripes, LLongMap* cache) {
    /*
    Gets the total number of designs possible for the given design, using different combinations of
    the available stripes.

    A design is possible if:
        1. The design starts with an available stripe pattern
        2. The rest of the pattern (minus the available stripe prefix) is possible.

    Encountered patterns are stored in the cache to speed things up.
    */
    // The base case, a design of length 0 is possible, you don't need any stripes to make it!
    if (designLength == 0) return 1;

    // Check if we've already encountered this design.
    long long cachePossible;
    if (getLLongMap(cache, design, &cachePossible)) {
        return cachePossible;
    }

    // If not, count up all the possible designs starting with each available stripe prefix.
    long long possible = 0;
    int stripeLength;
    for (int idx = 0; idx < availableStripes->numItems; idx += 1) {
        if (startsWith(design, availableStripes->data[idx])) {
            stripeLength = strlen(availableStripes->data[idx]);
            // Copy the design into a substring to not override keys in the map.
            char* subString = malloc(designLength - stripeLength + 1);
            memcpy(subString, design + stripeLength, designLength - stripeLength);
            subString[designLength - stripeLength] = '\0';

            possible += designsPossible(subString, designLength - stripeLength, availableStripes, cache);
        }
    }

    setLLongMap(cache, design, possible);

    return possible;
}

void problem1(char* inputFilePath) {
    /*
    Count the number of supplied patterns (like "rbrrwgu") that can possibly be
    constructred from a list of sub-patterns (like "r,rb,g,gu,wgu").

    Using recursion, a pattern is possible if it starts with any of the supplied
    sub-patterns (like "rbrrwgu" starts with "r") AND the rest of the pattern is
    a possible pattern (the recursive check for "brrwgy").

    To speed things up, a hash map is used to store the number of ways you can possibly
    construct the patterns already figured out.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Load the available stripes.
    StringArray availableStripes;
    initStringArray(&availableStripes, 100, 10);
    lineLen = getline(&line, &lineCap, inputFile);

    int wordEndIdx = 0;
    while (wordEndIdx < lineLen - 1) insertStringArray(&availableStripes, parseWord(line, wordEndIdx, &wordEndIdx));

    // Skip the empty line
    getline(&line, &lineCap, inputFile);

    // Possible designs cache
    LLongMap designCache;
    initLLongMap(&designCache);

    // Process the needed designs
    int possibleDesigns = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        // If there's a new line ending the line, replace it with a NULL terminator, otherwise
        // we need to make more space in the new buffer to add the NULL terminator.
        if (line[lineLen - 1] != '\n') lineLen += 1;
    
        // Copy the design into a substring to not override keys in the map.
        char* newLine = malloc(lineLen);
        memcpy(newLine, line, lineLen);
        newLine[lineLen - 1] = '\0';

        if (designsPossible(newLine, lineLen - 1, &availableStripes, &designCache) > 0) possibleDesigns += 1;
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", possibleDesigns, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Like part 1, but this time counting all the possible ways to make the patterns,
    not just the count of what patterns are possible.

    Uses the same exact algorithm as part 1.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // Load the available stripes.
    StringArray availableStripes;
    initStringArray(&availableStripes, 100, 10);
    lineLen = getline(&line, &lineCap, inputFile);

    int wordEndIdx = 0;
    while (wordEndIdx < lineLen - 1) insertStringArray(&availableStripes, parseWord(line, wordEndIdx, &wordEndIdx));

    // Skip the empty line
    getline(&line, &lineCap, inputFile);

    // Possible designs cache
    LLongMap designCache;
    initLLongMap(&designCache);

    // Process the needed designs
    long long possibleDesigns = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        // If there's a new line ending the line, replace it with a NULL terminator, otherwise
        // we need to make more space in the new buffer to add the NULL terminator.
        if (line[lineLen - 1] != '\n') lineLen += 1;

        // Copy the design into a substring to not override keys in the map.
        char* newLine = malloc(lineLen);
        memcpy(newLine, line, lineLen);
        newLine[lineLen - 1] = '\0';

        possibleDesigns += designsPossible(newLine, lineLen - 1, &availableStripes, &designCache);
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %lld [%.2fms]\n", possibleDesigns, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
