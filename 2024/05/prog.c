#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../utils/array.c"
#include "../../utils/string.c"

#define MAX_PAGE_NUMBER 100

// Global only because I need it for the comparison function needed for problem 2.
//
// The page order rules are stored as a 2D array, where the index of each row corresponds to
// a page number in the ruleset, and it's columns represent the page numbers the row's page
// number must come before. If a column's value is 0, there is no rule for that page number
// combination, and if it's 1 there is.
//
// So, a ruleset consisting only of the rules 2|1, 2|2, 1|1 would result in:
//
//  [0, 0, 0]
//  [0, 1, 0]
//  [0, 1, 1]
//
// Checking pageOrderRules[2][1] will tell you that 2 must be before 1.
//
// The input generally doesn't have page number of 0, so the first row and col will be all
// zeroes.
int pageOrderRules[MAX_PAGE_NUMBER + 1][MAX_PAGE_NUMBER + 1] = {0};

int compare_by_page_rules(const void* va, const void* vb) {
    int a = *(int*)va, b = *(int*)vb;
    return pageOrderRules[a][b] == 1 ? 1 : -1;
}

void problem1(char* inputFilePath) {
    /*
    The problem is asking to check if an update of page ordering is valid, based on a set of rules
    specifying which pages must come before other pages.

    The general algorithm is to compile the input ruleset into a 2D array, storing each page number
    and the list of other page numbers it must come before.

    Then when going through the page updates, we refer to the rule set checking each number against
    all previous numbers in the update to make sure they can all come before.
    */
    clock_t start = clock();

    // Zero out the rules.
    memset(pageOrderRules, 0, sizeof(pageOrderRules));

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // First, we process the page rules.
    int pageNumberParserEndIdx, rowNum, colNum;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        // The rules and updates are separated by an empty line, which has a length of 1.
        if (lineLen == 1) break;

        rowNum = parseNumber(line, 0, &pageNumberParserEndIdx);
        // + 1 to skip the comma separating the two numbers.
        colNum = parseNumber(line, pageNumberParserEndIdx + 1, &pageNumberParserEndIdx);
        pageOrderRules[rowNum][colNum] = 1;
    }

    // Then, we check the validity of the page updates.
    IntArray foundNumbers;
    initIntArray(&foundNumbers, 30);
    int pageNumber;
    bool isValid;
    int middlePageNumberSum = 0;
    // For each update, we'll check each number sequentially to ensure that it shouldn't come after any of
    // the previous numbers in the update. We can do this easily by checking the rules array, if a previous
    // number has a 1 in the current page number's row, the update is invalid.
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        pageNumberParserEndIdx = 0;
        foundNumbers.numItems = 0;
        isValid = true;

        // Parse each number
        while (pageNumberParserEndIdx < lineLen - 1) {
            pageNumber = parseNumber(line, pageNumberParserEndIdx, &pageNumberParserEndIdx);

            // Check all previously found numbers to make sure they can't come before the current one.
            for (int idx = 0; idx < foundNumbers.numItems; idx += 1) {
                if (pageOrderRules[pageNumber][foundNumbers.data[idx]] == 1) {
                    // Things are out of order, mark this update invalid and break out early.
                    isValid = false;
                    break;
                }
            }

            // Early break for efficiency.
            if (!isValid) break;

            insertIntArray(&foundNumbers, pageNumber);
        }

        // Only sum the middles of the valid updates.
        if (isValid) middlePageNumberSum += foundNumbers.data[foundNumbers.numItems / 2];
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 01: %d [%.2fms]\n", middlePageNumberSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void problem2(char* inputFilePath) {
    /*
    Similar to problem 1, this is asking to fix the ordering of the invalid updates.

    We determine the validity of updates exactly the same as part 1, but afterwards we sort the invalid
    update using the page rules to fix the order.
    */
    clock_t start = clock();

    // Zero out the rules.
    memset(pageOrderRules, 0, sizeof(pageOrderRules));

    FILE* inputFile = fopen(inputFilePath, "r");
    char* line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;

    // First, we process the page rules.
    int pageNumberParserEndIdx, rowNum, colNum;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        // The rules and updates are separated by an empty line, which has a length of 1.
        if (lineLen == 1) break;

        rowNum = parseNumber(line, 0, &pageNumberParserEndIdx);
        // + 1 to skip the comma separating the two numbers.
        colNum = parseNumber(line, pageNumberParserEndIdx + 1, &pageNumberParserEndIdx);
        pageOrderRules[rowNum][colNum] = 1;
    }

    // Then, we check the validity of the page updates.
    IntArray foundNumbers;
    initIntArray(&foundNumbers, 30);
    int pageNumber;
    bool isValid;
    int fixedMiddlePageNumberSum = 0;
    // For each update, we'll check each number sequentially to ensure that it shouldn't come after any of
    // the previous numbers in the update. We can do this easily by checking the rules array, if a previous
    // number has a 1 in the current page number's row, the update is invalid.
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        pageNumberParserEndIdx = 0;
        foundNumbers.numItems = 0;
        isValid = true;

        // Parse each number
        while (pageNumberParserEndIdx < lineLen - 1) {
            pageNumber = parseNumber(line, pageNumberParserEndIdx, &pageNumberParserEndIdx);

            // Check all previously found numbers to make sure they can't come before the current one.
            for (int idx = 0; idx < foundNumbers.numItems; idx += 1) {
                // If things are out of order, mark this update invalid but keep going to add the remaining
                // numbers to the foundNumbers array. This is inefficient but whatever.
                if (pageOrderRules[pageNumber][foundNumbers.data[idx]] == 1) isValid = false;
            }

            insertIntArray(&foundNumbers, pageNumber);
        }

        if (!isValid) {
            // If the list is invalid, we can simply sort by the page rules, since the page rules is
            // a total ordering.
            qsort(foundNumbers.data, foundNumbers.numItems, sizeof(int), compare_by_page_rules);
            fixedMiddlePageNumberSum += foundNumbers.data[foundNumbers.numItems / 2];
        }
    }

    fclose(inputFile);

    clock_t end = clock();
    printf("Problem 02: %d [%.2fms]\n", fixedMiddlePageNumberSum, (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

/*
Usage: prog <input_file_path>
*/
int main(int argc, char** argv) {
    problem1(argv[1]);
    problem2(argv[1]);

    return 0;
}
