#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

// Bit twiddling hacks, stackoverflow

uint8_t calcParityBit (uint8_t v);
void lagrangeInterpolate(uint8_t * currentSecretBlock, uint8_t * currentXs, uint8_t * currentYs, int k);
void gaussInterpolate(uint8_t * currentSecretBlock, uint8_t * Xs, uint8_t * Ys, int k);
int hasEqualBytes(uint8_t * bytes, int k);

#endif