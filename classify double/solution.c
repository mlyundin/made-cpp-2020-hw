#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>


/**
 * Library-level functions.
 * You should use them in the main sections.
 */

uint64_t convertToUint64 (double number) {
    return *((uint64_t *)(&number));
}

double convertToDouble (uint64_t number) {
    return *((double *)(&number));
}

bool getBit (const uint64_t number, const uint8_t index) {
    uint64_t mask = 1;
    return number & (mask << index);
}

uint64_t getOnesMask (const uint8_t start_index, const uint8_t end_index) {
    uint8_t n = end_index - start_index;
    uint64_t res = n == 64 ? 0 : 1;

    res <<= n;
    res -= 1;
    res <<= start_index;

    return res;
}

#define FRACTION_START_BIT 0
#define FRACTION_END_BIT 52
#define EXPONENT_START_BIT 52
#define EXPONENT_END_BIT 63
#define SING_BIT  63

bool checkForPlusZero (uint64_t number) {
    return number == 0;
}

bool checkForMinusZero (uint64_t number) {
    return number == 0x8000000000000000;
}

bool checkForPlusInf (uint64_t number) {
    return number == getOnesMask(EXPONENT_START_BIT, EXPONENT_END_BIT);
}

bool checkForMinusInf (uint64_t number) {
    return number == getOnesMask(EXPONENT_START_BIT, SING_BIT + 1);
}

bool checkForPlusNormal (uint64_t number) {
    uint64_t  exponent_part = getOnesMask(EXPONENT_START_BIT, EXPONENT_END_BIT) & number;
    return !getBit(number, SING_BIT) && exponent_part != 0 && exponent_part != getOnesMask(EXPONENT_START_BIT, EXPONENT_END_BIT);
}

bool checkForMinusNormal (uint64_t number) {
    uint64_t exponent_part = getOnesMask(EXPONENT_START_BIT, EXPONENT_END_BIT) & number;
    return getBit(number, SING_BIT) && exponent_part != 0 && exponent_part != getOnesMask(EXPONENT_START_BIT, EXPONENT_END_BIT);
}

bool checkForPlusDenormal (uint64_t number) {
    uint64_t fraction_part = getOnesMask(FRACTION_START_BIT, FRACTION_END_BIT) & number;
    uint64_t exponent_part = getOnesMask(EXPONENT_START_BIT, EXPONENT_END_BIT) & number;
    return !getBit(number, SING_BIT) && fraction_part != 0 && exponent_part == 0;
}

bool checkForMinusDenormal (uint64_t number) {
    uint64_t fraction_part = getOnesMask(FRACTION_START_BIT, FRACTION_END_BIT) & number;
    uint64_t exponent_mask = getOnesMask(EXPONENT_START_BIT, EXPONENT_END_BIT) & number;
    return getBit(number, SING_BIT) && fraction_part != 0 && exponent_mask == 0;
}

bool checkForSignalingNan (uint64_t number) {
    uint64_t fraction_part = getOnesMask(FRACTION_START_BIT, FRACTION_END_BIT) & number;
    uint64_t exponent_mask = getOnesMask(EXPONENT_START_BIT, EXPONENT_END_BIT);
    return number & exponent_mask == exponent_mask && fraction_part != 0 &&
            !getBit(fraction_part, FRACTION_END_BIT - 1);
}

bool checkForQuietNan (uint64_t number) {
    uint64_t mask = getOnesMask(EXPONENT_START_BIT - 1, EXPONENT_END_BIT);
    return number & mask == mask;
}

void classify (double number) {
    if (checkForPlusZero(convertToUint64(number))) {
        printf("Plus zero\n");
    }

    else if (checkForMinusZero(convertToUint64(number))) {
        printf("Minus zero\n");
    }

    else if (checkForPlusInf(convertToUint64(number))) {
        printf("Plus inf\n");
    }

    else if (checkForMinusInf(convertToUint64(number))) {
        printf("Minus inf\n");
    }

    else if (checkForPlusNormal(convertToUint64(number))) {
        printf("Plus normal\n");
    }

    else if (checkForMinusNormal(convertToUint64(number))) {
        printf("Minus normal\n");
    }

    else if (checkForPlusDenormal(convertToUint64(number))) {
        printf("Plus Denormal\n");
    }

    else if (checkForMinusDenormal(convertToUint64(number))) {
        printf("Minus Denormal\n");
    }

    else if (checkForSignalingNan(convertToUint64(number))) {
        printf("Signailing NaN\n");
    }

    else if (checkForQuietNan(convertToUint64(number))) {
        printf("Quiet NaN\n");
    }

    else {
        printf("Error.\n");
    }
}

int main() {
    classify(convertToDouble(getOnesMask(0, 0)));
    classify(convertToDouble(getOnesMask(SING_BIT, SING_BIT + 1)));

    classify(convertToDouble(getOnesMask(FRACTION_START_BIT + 3, FRACTION_END_BIT - 4)));
    classify(convertToDouble(getOnesMask(FRACTION_START_BIT + 3, FRACTION_END_BIT - 4)
        + getOnesMask(SING_BIT, SING_BIT + 1)));

    classify(convertToDouble(getOnesMask(EXPONENT_START_BIT + 3, EXPONENT_END_BIT - 4)));
    classify(convertToDouble(getOnesMask(EXPONENT_START_BIT + 3, SING_BIT + 1)));

    classify(convertToDouble(getOnesMask(EXPONENT_START_BIT, EXPONENT_END_BIT)));
    classify(convertToDouble(getOnesMask(EXPONENT_START_BIT, EXPONENT_END_BIT + 1)));

    classify(convertToDouble(getOnesMask(EXPONENT_START_BIT, EXPONENT_END_BIT) + 1));
    classify(convertToDouble(getOnesMask(EXPONENT_START_BIT, EXPONENT_END_BIT) + 1
    + getOnesMask(SING_BIT, SING_BIT + 1)));

    classify(convertToDouble(getOnesMask(EXPONENT_START_BIT - 1, EXPONENT_END_BIT) + 1));
    classify(convertToDouble(getOnesMask(EXPONENT_START_BIT - 1, EXPONENT_END_BIT) + 1
                                 + getOnesMask(SING_BIT, SING_BIT + 1)));
}
