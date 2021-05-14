#include "../include/utils.h"
#include "../include/galoisFields.h"

// Bit twiddling hacks, stackoverflow


char calcParityBit (unsigned char v)
{
    return (0x6996u >> ((v ^ (v >> 4)) & 0xf)) & 1;
}

void lagrangeInterpolate(uint8_t * currentSecretBlock, uint8_t * Xs, uint8_t * Ys, int k) {
    uint8_t sumAux = 0x0;
    uint8_t multAux = 0x1;

    // get first coefficient
    for (int i = 0; i < k; i ++ ) {
        for (int q = 0; q < k; q++) {
            if (q != i) {
                multAux = multiplyModGenP(multAux, dividePolynomials(Xs[q], multiplyModGenP(Xs[i], Xs[q])));
            }
        }
        sumAux ^= multiplyModGenP(Ys[i], multAux);
        multAux = 0x1;
    }
    currentSecretBlock[0] = sumAux;

    uint8_t yaux;
    for (int kind = 1; kind < k; kind ++) {
        uint8_t sumAux = 0x0;
        yaux = dividePolynomials(multiplyModGenP(Ys[kind], currentSecretBlock[0]), Xs[kind]);
        for (int i = 0; i < k; i ++ ) {
            for (int q = 0; q < k; q++) {
                if (q != i) {
                    multAux = multiplyModGenP(multAux, dividePolynomials(Xs[q], multiplyModGenP(Xs[i], Xs[q])));
                }
            }
            sumAux ^= multiplyModGenP(yaux, multAux);
            multAux = 0x1;
        }
        currentSecretBlock[kind] = sumAux;
    }
}