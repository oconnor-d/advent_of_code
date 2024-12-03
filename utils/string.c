#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * Gets the first whole number from `string`, if any is present. The `startIdx` and `endIdx`
 * will hold the starting and ending index of the found whole number.
 *
 * If there is no whole number in the string, -1 will be returned and `startIdx` and `endIdx` will
 * both hold -1.
 */
long long parseFirstNumber(char* string, int* startIdx, int* endIdx) {
    char* strPtr = string;
    while (*strPtr) {
        if (isdigit(*strPtr) || *strPtr == '-') {
            *startIdx = strPtr - string;
            long long val = strtoll(strPtr, &strPtr, 10);
            *endIdx = strPtr - string;

            return val;
        } else {
            strPtr += 1;
        }
    }

    *startIdx = -1;
    *endIdx = -1;
    return -1;
}

/**
 * Parses the first number in the given string starting from `startIdx` (inclusive). The idx (exclusive) of
 * the end of the parsed number is stored in `endIdx`.
 *
 * If no number is found, returns 0 and -1 is stored in `endIdx`. To check for successful parsing, you
 * MUST check that `endIdx` isn't -1.
 *
 * Args:
 *  string (char*): the string to parse.
 *  startIdx (int): the index to start looking from (inclusive).
 *  endIdx (*int): the pointer to store the end index (exclusive) of the found number.
 */
long long parseNumber(char* string, int startIdx, int* endIdx) {
    char* strPtr = string + startIdx;

    while (*strPtr) {
        if (isdigit(*strPtr) || *strPtr == '-') {
            long long val = strtoll(strPtr, &strPtr, 10);
            *endIdx = strPtr - string;

            return val;
        } else {
            strPtr += 1;
        }
    }

    *endIdx = -1;
    return 0;
}

// TODO: Implement proper edge case handling before using, like: "    ".
/**
 * Gets the first whole word from `string`, if any is present. The `startIdx` and `endIdx`
 * will hold the starting and ending index of the found word.
 *
 * A word is a continuous set of non-empty and non-newline chars.
 */
char* parseFirstWord(char* string, int* startIdx, int* endIdx) {
    char* firstWord = string;
    bool wordStarted = false;
    int idx, firstWordIdx = 0;
    for (idx = 0; idx < strlen(string); idx += 1) {
        if (wordStarted && (string[idx] == ' ' || string[idx] == '\n')) {
            *endIdx = idx;
            firstWord[firstWordIdx] = '\0';
            return firstWord;
        } else if (!wordStarted && (string[idx] == ' ' || string[idx] == '\n')) {
            wordStarted = true;
            *startIdx = idx;
        } else {
            wordStarted = true;
            firstWord[firstWordIdx] = string[idx];
            firstWordIdx += 1;
        }
    };

    *endIdx = idx;
    firstWord[firstWordIdx] = '\0';
    return firstWord;
}

/**
 * Loads a substring of `string` from `startIdx` to `endIdx` into the `dest` buffer, and
 * null terminates `dest`.
 *
 * Make sure the `dest` buffer is big enough to store the whole substring + null terminator.
 */
void substring(char* string, char* dest, int startIdx, int endIdx) {
    strncpy(dest, string + startIdx, endIdx - startIdx);
    dest[endIdx - startIdx] = '\0';
}

/**
 * Builds a new null-terminated char ptr to a version of the given string with all
 * whitespace removed.
 */
char* stripWhitespace(char* string) {
    char* strippedString = string;

    int sourceIdx, destIdx;
    for (sourceIdx = 0, destIdx = 0; sourceIdx < strlen(string); sourceIdx += 1, destIdx += 1) {
        if (string[sourceIdx] != ' ') {
            strippedString[destIdx] = string[sourceIdx];
        } else {
            // Basically cancel out the += 1 from the next loop of this.
            destIdx -= 1;
        }
    }
    strippedString[destIdx] = '\0';

    return strippedString;
}
