/*
Counts the number of digits in the given number.

Args:
    number (long): the number to count the digits of.
*/
int countDigits(long number) {
    if (number < 0) number *= -1;

    int digits = 1;
    while (number > 9) {
        number /= 10;
        digits += 1;
    }

    return digits;
}

/*
Gets the right half of the number. The number of digits in the number
MUST be even, otherwise this will not work reliably.

Args:
    number (long): the number to get the left half of.
    digits (int): the number of digits of the number
*/
long rightHalf(long number, int digits) {
    // Manually compute power since `pow` doesn't return an int.
    int power = 10;
    for (int idx = 1; idx < digits / 2; idx += 1)  power *= 10;

    return number % power;
}

/*
Gets the left half of the number. The number of digits in the number
MUST be even, otherwise this will not work reliably.

Args:
    number (long): the number to get the left half of.
    digits (int): the number of digits of the number
*/
long leftHalf(long number, int digits) {
    // Manually compute power since `pow` doesn't return an int.
    int power = 10;
    for (int idx = 1; idx < digits / 2; idx += 1) power *= 10;

    return number / power;
}