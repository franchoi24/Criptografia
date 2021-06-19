#include "../include/cryptosystem.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static void printUsageThenExit() {
    printf("Usage: ./ss [d|r] secretImagePath k shadesPath\n");
    exit(1);
}

int main (int argc, char * argv[]) {

    if (argc < 5) {
        printUsageThenExit();
    }
    int k = argv[3][0] - 48;
    if (k < 4 || k > 6) {
        printf("Error: k must be between 4 and 6 \n");
        exit(0);
    }

    if (strcmp(argv[1], "d") == 0) {
        encrypt(argv[2], k, argv[4]);
    } else if (strcmp(argv[1], "r") == 0) {
        decrypt(argv[2], k, argv[4]);
    } else {
        printUsageThenExit();
    }
}