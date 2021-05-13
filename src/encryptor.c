#include <stdlib.h>
#include <stdio.h>
#include "../include/bmpProcessing.h"

int main (int argc, char *argv[]) {
    BITMAPINFOHEADER secretBmpIH = {0};
    uint8_t *secretBitmapData;
    secretBitmapData = LoadBitmapFile("res/Audrey.bmp",&secretBmpIH);

    int k = 5;
    int secretBlockCount = secretBmpIH.biSizeImage / k;

    // Read shade image

    BITMAPINFOHEADER shadeBmpIH = {0};
    uint8_t * shadeBitmapData = LoadBitmapFile("res/Alfred.bmp", &shadeBmpIH);

    // Adjust shade image to rubric specification

    uint8_t * currentBlockStart;
    for (int currentBlockNo = 0; currentBlockNo < secretBlockCount; ++currentBlockNo) {
        
    }

    return 0;
}