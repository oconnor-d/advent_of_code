#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/string.c"

void problem1(char* inputFilePath) {
    /*
    I started trying to get the counts of each box, just to see if there's enough space
    for them in the region at the start. I substituted 9 for all the sizes as the max
    present size instead of that and even that seemed to work!

    Less nice inputs will almost definitely fail on this one.
    */
    clock_t start = clock();

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    int endIdx;
    int area;
    int valid = 0;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        area = parseNumber(line, 0, &endIdx) * parseNumber(line, endIdx, &endIdx);

        if (area == 0) continue;

        int total = 0;
        while (endIdx <= lineLen - 2) total += parseNumber(line, endIdx, &endIdx) * 9;

        if (total <= area) valid += 1;
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", valid, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);

    return 0;
}
