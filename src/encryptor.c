#include <stdlib.h>
#include <stdio.h>
#include "../include/bmpProcessing.h"

static void turnRowsUpsideDown(uint8_t * bitmapData, BITMAPINFOHEADER infoHeader) {
    int width = infoHeader.biWidth;
    int height = infoHeader.biHeight;

    int upperIndex, lowerIndex;
    uint8_t tmp;

    for (int h = 0; h < height/2; h++) {
        upperIndex = h;
        lowerIndex = height - 1 - h;
        for (int i = 0; i < width; i++) {
            // just swap row bytes
            tmp = bitmapData[upperIndex * width + i];
            bitmapData[upperIndex * width + i] = bitmapData[lowerIndex * width + i];
            bitmapData[lowerIndex * width + i] = tmp;
        }
    }
}

int main (int argc, char *argv[]) {
    BITMAPINFOHEADER secretBmpIH = {0};
    uint8_t *secretBitmapData;
    secretBitmapData = LoadBitmapFile("res/Audrey.bmp",&secretBmpIH);

    int k = 5;
    int secretBlockCount = secretBmpIH.biSizeImage / k;

    // For each shade image

        // Read shade image

        BITMAPINFOHEADER shadeBmpIH = {0};
        uint8_t * shadeBitmapData = LoadBitmapFile("res/Alfred.bmp", &shadeBmpIH);

        // Adjust shade image to rubric specification
        turnRowsUpsideDown(shadeBitmapData, shadeBmpIH);
        // calculate number of 2x2 blocks that fit horizontal line in shade image
        int shadeXBlockNo = shadeBmpIH.biWidth / 2;
        uint8_t * currentBlockStart;
        for (int currentBlockNo = 0; currentBlockNo < secretBlockCount; ++currentBlockNo, currentBlockStart += k) {
            // for each blockNo, get XUVW. Then just calculate F(X) and replace where necessary
            int Xind, Uind, Vind, Wind;
            // just do the math
            Xind =  (currentBlockNo / shadeXBlockNo) * 2 * shadeBmpIH.biWidth + (currentBlockNo % shadeXBlockNo) * 2;
            Vind = ((currentBlockNo / shadeXBlockNo) * 2 + 1) * shadeBmpIH.biWidth + (currentBlockNo % shadeXBlockNo) * 2;
            Uind = Vind + 1;
            Wind = Xind + 1;

            // Now, evaluate polynomial in Xind
                 
        }

    return 0;
}