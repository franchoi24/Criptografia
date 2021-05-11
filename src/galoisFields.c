#include <stdint.h>

const uint16_t generatingPolynomial = 0x0163;

// For tidiness purposes only
uint16_t sumPolynomials(uint16_t p1, uint16_t p2) {
    return p1^p2;
}

