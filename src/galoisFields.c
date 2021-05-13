#include <stdint.h>
#include "../include/galoisFields.h"
#include <stdio.h>
#include <malloc.h>

static const uint16_t generatingPolynomial = 0x0163;
static const int CARDINALITY = 256;
static const uint8_t WORD_BIT_LENGTH = 16;


// For tidiness purposes only
uint16_t sumPolynomials(uint16_t p1, uint16_t p2) {
    return p1^p2;
}

// Find degree (aka index of most significant bit set to 1)
static uint8_t findDegree(uint16_t p) {
    int d;
    for (d = 0; d < 16 && p != 0; d++) {
        p = p >> 1;
    }
    return (d==16) ? d-2: ((d==0) ? d :  d-1);
}

uint16_t multiplyModGenP (uint16_t p1, uint16_t p2) {

    // Multiplication part
    // find bits set to 1; first find largest polynomial
    uint16_t sP, lP; 
    if (p1 > p2) {
        lP = p1; sP = p2;
    } else {
        lP = p2; sP = p1;
    }

    uint16_t ithBit;
    uint16_t xorValue = 0x0000;
    for (uint8_t i = 0; i < WORD_BIT_LENGTH; i++) {
        ithBit = (sP >> i) << (WORD_BIT_LENGTH -1);
        ithBit = ithBit >> (WORD_BIT_LENGTH -1);
        switch (ithBit) {
            case 0:
                break;
            case 1:
                xorValue = xorValue ^ (lP<<i);
                break;
            default:
                printf("Error multiplying - shifting bits from largest polynomial");
                exit(1);
                //break;
        }
    }

    // Reduction part

    uint16_t rest = xorValue;
    uint16_t genPDegree = findDegree(generatingPolynomial);
    uint16_t restDegree = findDegree(rest);
    while (restDegree - genPDegree >= 0) {
        rest = rest ^ (generatingPolynomial << (restDegree-genPDegree));
        restDegree = findDegree(rest);
    }

    return rest;

}

uint16_t galoisPower(uint16_t x, int pow) {
    uint16_t result = 0x0001;
    for (int i = 0; i < pow; i++) {
        result = multiplyModGenP(result, x);
    }
    return result;
}

uint16_t dividePolynomials (uint16_t dividend, uint16_t divisor) {

    for (uint16_t i = 0; i < CARDINALITY; i++){
        // Logic behind this: find mult inverse of divisor
        if (multiplyModGenP(i, divisor) == 1) {
            // Then multiply dividend by it
            return multiplyModGenP(dividend, i);
        }
    }
}

// int main (int argc, char * argv[]) {
//     uint16_t x = 0x0163;
//     uint16_t y;

//     multiplyModGenP(0x0054, 0x001D);
    
//     return 0;
// }