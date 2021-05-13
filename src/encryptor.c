#include <stdlib.h>
#include <stdio.h>
#include "../include/bmpProcessing.h"
#include "../include/galoisFields.h"

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

// Bit twiddling hacks, stackoverflow

char calcParityBit (unsigned char v)
{
    return (0x6996u >> ((v ^ (v >> 4)) & 0xf)) & 1;
}


int main (int argc, char *argv[]) {
    BITMAPINFOHEADER secretBmpIH = {0};
    BITMAPFILEHEADER secretBmpFH = {0};
    uint8_t *secretBitmapData;
    secretBitmapData = LoadBitmapFile("res/Audrey.bmp",&secretBmpFH,&secretBmpIH);

    int k = 5;
    int secretBlockCount = secretBmpIH.biSizeImage / k;

    // For each shade image

        // Read shade image

        BITMAPINFOHEADER shadeBmpIH = {0};
        BITMAPFILEHEADER shadeBmpFH = {0};
        uint8_t * shadeBitmapData = LoadBitmapFile("res/Alfred.bmp", &shadeBmpFH, &shadeBmpIH);

        // Adjust shade image to rubric specification
        turnRowsUpsideDown(shadeBitmapData, shadeBmpIH);
        // calculate number of 2x2 blocks that fit horizontal line in shade image
        int shadeXBlockNo = shadeBmpIH.biWidth / 2;
        uint8_t * currentBlockStart = secretBitmapData;

        int Xind, Uind, Vind, Wind;
        uint8_t FxByte;

        uint8_t newW;
        uint8_t newV;
        uint8_t newU;
        uint16_t F_X;

        for (int currentBlockNo = 0; currentBlockNo < secretBlockCount; ++currentBlockNo) {
            // for each blockNo, get XUVW. Then just calculate F(X) and replace where necessary
            // just do the math, maybe it gets easier to compute with value of the prev iteration
            Xind =  (currentBlockNo / shadeXBlockNo) * 2 * shadeBmpIH.biWidth + (currentBlockNo % shadeXBlockNo) * 2;
            Vind = ((currentBlockNo / shadeXBlockNo) * 2 + 1) * shadeBmpIH.biWidth + (currentBlockNo % shadeXBlockNo) * 2;
            Uind = Vind + 1;
            Wind = Xind + 1;

            // Now, evaluate polynomial in Xind
            F_X = 0x0000;
            for (int pow = 0; pow < k; pow++) {
                F_X ^= multiplyModGenP((uint16_t) currentBlockStart[pow], galoisPower((uint16_t) shadeBitmapData[Xind], pow) );
            }
            FxByte = (uint8_t) F_X;
        
            // Having F_X, change values for U, V and W

            // first calculate them here for clarity
            newW = (shadeBitmapData[Wind] & 0b11111000) | ((FxByte & 0b11100000)>>5); // Three highest bits of FX in 3 lowest bits of ~W
            newV = (shadeBitmapData[Vind] & 0b11111000) | ((FxByte & 0b00011100)>>2); // Second group of 3 highest bits of FX in 3 lower bits of ~V
            newU = (shadeBitmapData[Uind] & 0b11111000) | (FxByte & 0b00000011) | (calcParityBit(FxByte) << 2); // 2 LB of FX in 2 LB of ~U but also parity in 3rd lowest bit of ~U
        
            // then write them off
            shadeBitmapData[Wind] = newW;
            shadeBitmapData[Vind] = newV;
            shadeBitmapData[Uind] = newU;
            currentBlockStart += k;
            printf("Progress: %g\n", currentBlockNo/(double)secretBlockCount);
        }

        // save the image to see if it works
        turnRowsUpsideDown(shadeBitmapData, shadeBmpIH);

        writeBitmapToFile("res/newImg.bmp", &shadeBmpFH, &shadeBmpIH, shadeBitmapData);


    return 0;
}