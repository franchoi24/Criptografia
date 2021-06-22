#include "../include/utils.h"
#include "../include/galoisFields.h"

// Bit twiddling hacks, stackoverflow


uint8_t calcParityBit (uint8_t v)
{
    return (0x6996u >> ((v ^ (v >> 4)) & 0xf)) & 1;
}

// Meant for debugging
int hasEqualBytes(uint8_t * bytes, int k) {
    int j;
    for (int i = 0; i < k-1; i++) {
        for (j = i+1; j < k; j++) {
            if (bytes[i] == bytes[j]) {
                return 1;
            }
        } 
    }
    return 0;
}

void lagrangeInterpolate(uint8_t * currentSecretBlock, uint8_t * Xs, uint8_t * Ys, int k) {
    uint8_t sumAux = 0x0;
    uint8_t multAux = 0x1;

    // get first coefficient
    for (int i = 0; i < k; i ++ ) {
        for (int q = 0; q < k; q++) {
            if (q != i) {
                multAux = multiplyModGenP(multAux, dividePolynomials(Xs[q], Xs[i] ^ Xs[q]));
            }
        }
        sumAux ^= multiplyModGenP(Ys[i], multAux);
        multAux = 0x1;
    }
    currentSecretBlock[0] = sumAux;

    uint8_t yaux;
    for (int r = 1; r < k; r ++) {
        sumAux = 0x0;
        yaux = dividePolynomials(
            Ys[r] ^ currentSecretBlock[0],
            Xs[r]);
        for (int i = 0; i < k - (r-1); i ++ ) {
            for (int q = 0; q < k - (r-1); q++) {
                if (q != i) {
                    multAux = multiplyModGenP(multAux, dividePolynomials(Xs[q], Xs[i] ^ Xs[q]));
                }
            }
            sumAux ^= multiplyModGenP(yaux, multAux);
            multAux = 0x1;
        }
        currentSecretBlock[r] = sumAux;
    }
}

void gaussInterpolate(uint8_t * currentSecretBlock, uint8_t * Xs, uint8_t * Ys, int k) {
    uint8_t sumAux = 0x0;

    uint8_t A[k][k+1];
    uint8_t c;

    // First, generate augmented matrix
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            A[i][j] = galoisPower(Xs[i], j);
        }
        A[i][k] = Ys[i];
    }

    for (int j = 0; j < k; j++) {
        for (int i = 0; i < k; i++) {
            if (i>j) {
                c = dividePolynomials(A[i][j], A[j][j]);
                for (int ii = 0; ii < k+1; ii++) {
                    A[i][ii] = A[i][ii] ^ multiplyModGenP(c, A[j][ii]);
                }
            }
        }
    }

    currentSecretBlock[k-1] = dividePolynomials(A[k-1][k], A[k-1][k-1]);

    // Backward substitution
    for (int i = k-2; i >= 0; i--) {
        sumAux = 0x0;
        for (int j = i+1; j < k; j++) {
            sumAux=sumAux^multiplyModGenP(A[i][j], currentSecretBlock[j]);
        }
        currentSecretBlock[i] = dividePolynomials(A[i][k]^sumAux, A[i][i]);
    }
}