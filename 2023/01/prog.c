#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


void problem_1() {
    FILE *inputFile = fopen("2023/01/input.txt", "r");

    int total = 0;
    int digit1, digit2;

    char *line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    while ((lineLen = getline(&line, &lineCap, inputFile)) > 0) {
        digit1 = digit2 = -1;

        for (int idx = 0; idx < lineLen; idx += 1) {
            if (isdigit(line[idx]) && digit1 == -1) {
                digit1 = line[idx] - '0';
            } else if (isdigit(line[idx])) {
                digit2 = line[idx] - '0';
            }
        }

        total += (digit1 * 10) + ((digit2 == -1) ? digit1 : digit2);
    }

    printf("Problem 01: %d\n", total);
}


void problem_2() {
    FILE* input_file = fopen("2023/01/input.txt", "r");
    
    int total = 0;

    // Used to convert the two find digits into an integer.
    char digit_1, digit_2;
    char digits[3];
    digits[2] = '\0';
    bool digit_1_found = false;
    bool digit_2_found = false;

    // seven is the single digit number with the most letters of 5 + null terminate.
    char number_word_array[6];

    char ch, found_digit;
    int look_ahead_idx = 0;
    while (true) {
        if (look_ahead_idx == 0) {
            // Clear out the array that holds the number being looked up.
            found_digit = '\0';
            for (int idx = 0; idx < 6; idx += 1) {
                number_word_array[idx] = '\0';
            }
        }

        ch = fgetc(input_file);

        if (isdigit(ch) && look_ahead_idx == 0) {
            // Not parsing a number word and found a digit, add it.
            found_digit = ch;
        } else if (look_ahead_idx == 0 && (ch == '\n' || ch == EOF)) {
            // Successfully processed the whole line, add the digits to the total.
            digits[0] = digit_1;

            if (digit_2_found) {
                digits[1] = digit_2;
            } else {
                // if there's only one digit in the line, the number is that digit twice.
                digits[1] = digit_1;
            }
            total += atoi(digits);

            digit_1_found = false;
            digit_2_found = false;

            if (ch == EOF) {
                break;
            }
        } else if (ch == '\n' || ch == EOF || look_ahead_idx > 5) {
            // Hit the end of the line, go back and start at the next index.
            fseek(input_file, -look_ahead_idx, SEEK_CUR);
            if (ch == EOF) {
                fseek(input_file, 1, SEEK_CUR);
            }
            look_ahead_idx = 0;
        } else {
            number_word_array[look_ahead_idx] = ch;

            // If a digit word is found, save the found digit and seek back to the
            // second character of the word.
            if (strcmp(number_word_array, "one") == 0) {
                found_digit = '1';
                fseek(input_file, -2, SEEK_CUR);
                look_ahead_idx = 0;
            } else if (strcmp(number_word_array, "two") == 0) {
                found_digit = '2';
                fseek(input_file, -2, SEEK_CUR);
                look_ahead_idx = 0;
            } else if (strcmp(number_word_array, "three") == 0) {
                found_digit = '3';
                fseek(input_file, -4, SEEK_CUR);
                look_ahead_idx = 0;
            } else if (strcmp(number_word_array, "four") == 0) {
                found_digit = '4';
                fseek(input_file, -3, SEEK_CUR);
                look_ahead_idx = 0;
            } else if (strcmp(number_word_array, "five") == 0) {
                found_digit = '5';
                fseek(input_file, -3, SEEK_CUR);
                look_ahead_idx = 0;
            } else if (strcmp(number_word_array, "six") == 0) {
                found_digit = '6';
                fseek(input_file, -2, SEEK_CUR);
                look_ahead_idx = 0;
            } else if (strcmp(number_word_array, "seven") == 0) {
                found_digit = '7';
                fseek(input_file, -4, SEEK_CUR);
                look_ahead_idx = 0;
            } else if (strcmp(number_word_array, "eight") == 0) {
                found_digit = '8';
                fseek(input_file, -4, SEEK_CUR);
                look_ahead_idx = 0;
            } else if (strcmp(number_word_array, "nine") == 0) {
                found_digit = '9';
                fseek(input_file, -3, SEEK_CUR);
                look_ahead_idx = 0;
            } else {
                look_ahead_idx += 1;
            }
        }

        if (isdigit(found_digit)) {
            if (!digit_1_found) {
                digit_1 = found_digit;
                digit_1_found = true;
            } else {
                digit_2 = found_digit;
                digit_2_found = true;
            }
        }
    }

    printf("Problem 02: %d\n", total);
}


int main() {
    clock_t start = clock();
    problem_1();
    clock_t end = clock();
    printf("Problem 01 Time: %fs\n\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    problem_2(); 
    end = clock();
    printf("Problem 02 Time: %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}
