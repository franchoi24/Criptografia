#include <stdlib.h>
#include <stdio.h>
#include "../include/bmpProcessing.h"
#include "../include/galoisFields.h"
#include "../include/utils.h"


int main2 () {
    BITMAPINFOHEADER secretBmpIH = {0};
    BITMAPFILEHEADER secretBmpFH = {0};
    BITMAPDATA secretBitmapData;
    secretBitmapData = LoadBitmapFile("res/Audrey.bmp",&secretBmpFH,&secretBmpIH);

    int k = 5;
    int secretBlockCount = secretBmpIH.biSizeImage / k;

    // For each shade image

        // Read shade image

        BITMAPINFOHEADER shadeBmpIH = {0};
        BITMAPFILEHEADER shadeBmpFH = {0};
        BITMAPDATA shadeBitmapData = LoadBitmapFile("res/James.bmp", &shadeBmpFH, &shadeBmpIH);

        // Adjust shade image to rubric specification
        turnRowsUpsideDown(shadeBitmapData.data, shadeBmpIH);
        // calculate number of 2x2 blocks that fit horizontal line in shade image

        int shadeXBlockNo = shadeBmpIH.biWidth / 2;
        uint8_t * currentBlockStart = secretBitmapData.data;

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
                F_X ^= multiplyModGenP((uint16_t) currentBlockStart[pow], galoisPower((uint16_t) shadeBitmapData.data[Xind], pow) );
            }
            FxByte = (uint8_t) F_X;
        
            // Having F_X, change values for U, V and W

            // first calculate them here for clarity
            newW = (shadeBitmapData.data[Wind] & 0b11111000) | ((FxByte & 0b11100000)>>5); // Three highest bits of FX in 3 lowest bits of ~W
            newV = (shadeBitmapData.data[Vind] & 0b11111000) | ((FxByte & 0b00011100)>>2); // Second group of 3 highest bits of FX in 3 lower bits of ~V
            newU = (shadeBitmapData.data[Uind] & 0b11111000) | (FxByte & 0b00000011) | (calcParityBit(FxByte) << 2); // 2 LB of FX in 2 LB of ~U but also parity in 3rd lowest bit of ~U
        
            // then write them off
            shadeBitmapData.data[Wind] = newW;
            shadeBitmapData.data[Vind] = newV;
            shadeBitmapData.data[Uind] = newU;
            currentBlockStart += k;
            printf("Progress: %g\n", currentBlockNo/(double)secretBlockCount);
        }

        // save the image to see if it works
        turnRowsUpsideDown(shadeBitmapData.data, shadeBmpIH);

        writeBitmapToFile("res/newImg2.bmp", &shadeBmpFH, &shadeBmpIH, shadeBitmapData);


    return 0;
}