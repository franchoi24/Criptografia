#include "../include/utils.h"

// Bit twiddling hacks, stackoverflow


char calcParityBit (unsigned char v)
{
    return (0x6996u >> ((v ^ (v >> 4)) & 0xf)) & 1;
}

void lagrangeInterpolate(uint8_t * currentSecretBlock, uint8_t * currentXs, uint8_t * currentYs, int k) {

}