#include <ctype.h>
#include <stdlib.h>
#include <string.h>


/**
 * Gets the first integer number from `string`, if any is present. The `startIdx` and `endIdx`
 * will hold the starting and ending index of the found integer.
 * 
 * If there is no integer in the string, -1 will be returned and `startIdx` and `endIdx` will
 * both hold -1.
*/
int parseFirstNumber(char* string, int* startIdx, int* endIdx) {
    char* strPtr = string;
    while (*strPtr) {
        if (isdigit(*strPtr)) {
            *startIdx = strPtr - string;
            long val = strtol(strPtr, &strPtr, 10);
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
 * Loads a substring of `string` from `startIdx` to `endIdx` into the `dest` buffer, and
 * null terminates `dest`.
 * 
 * Make sure the `dest` buffer is big enough to store the whole substring + null terminator.
*/
void substring(char* string, char* dest, int startIdx, int endIdx) {
    strncpy(dest, string + startIdx, endIdx - startIdx);
    dest[endIdx - startIdx] = '\0';
}
