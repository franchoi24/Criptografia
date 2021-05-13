#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

// Bit twiddling hacks, stackoverflow

char calcParityBit (unsigned char v);
void lagrangeInterpolate(uint8_t * currentSecretBlock, uint8_t * currentXs, uint8_t * currentYs, int k);

#endif