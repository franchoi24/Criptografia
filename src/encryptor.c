#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "../include/bmpProcessing.h"
#include "../include/galoisFields.h"
#include "../include/utils.h"


int main1 () {
    BITMAPINFOHEADER secretBmpIH = {0};
    BITMAPFILEHEADER secretBmpFH = {0};
    BITMAPDATA secretBitmapData;
    secretBitmapData = LoadBitmapFile("res/Albert.bmp",&secretBmpFH,&secretBmpIH);

    int k = 5;
    int n = 6;
    int secretBlockCount = secretBmpIH.biSizeImage / k;

    char shadesDirName[512] = "res/shadesBefore";
    char outputDirName[512] = "res/shades";
    if (shadesDirName[strlen(shadesDirName)-1] != '/') {
        strcpy(shadesDirName+strlen(shadesDirName), "/");
    }
    if (outputDirName[strlen(outputDirName)-1] != '/') {
        strcpy(outputDirName+strlen(outputDirName), "/");
    }

    BITMAPINFOHEADER shadesInfoHeaders[n];
    BITMAPFILEHEADER shadesFileHeaders[n];
    BITMAPDATA shadesBitmapData[n];

    char shadesPathList[n][600];

    DIR * d;
    struct dirent * dir;
    d = opendir(shadesDirName);
    if (d) {
        char filePath[600];
        strcpy(filePath, shadesDirName);
        int index = 0;
        while ((dir = readdir(d)) != NULL && index < n) {
            if (strstr(dir->d_name, ".bmp") > 0) {
                strcpy(filePath + strlen(shadesDirName), dir->d_name);
                // Read shade image
                shadesBitmapData[index] = LoadBitmapFile(filePath, &(shadesFileHeaders[index]), &(shadesInfoHeaders[index]));
                // Adjust shade image to rubric specification
                turnRowsUpsideDown(shadesBitmapData[index].data, shadesInfoHeaders[index]);
                sprintf(shadesPathList[index], "%s%s", outputDirName, dir->d_name);
                index++;
            }
        }
        closedir(d);
    }


        int shadeXBlockNo = shadesInfoHeaders[0].biWidth / 2;
        uint8_t * currentBlockStart = secretBitmapData.data;

        int Xind, Uind, Vind, Wind;
        uint8_t FxByte;

        uint8_t newW;
        uint8_t newV;
        uint8_t newU;
        uint16_t F_X;

        uint8_t currentEvaluatedFs[n];

        for (int currentBlockNo = 0; currentBlockNo < secretBlockCount; ++currentBlockNo) {
            
            // Correct for all different Xs

            for (int nind = 0; nind < n; ++nind) {
                Xind =  (currentBlockNo / shadeXBlockNo) * 2 * shadesInfoHeaders[nind].biWidth + (currentBlockNo % shadeXBlockNo) * 2;
                for (int nind2 = 0; nind2 < n; ++nind2) {
                    // Clearly separate cases of conflict
                    if(nind != nind2 && shadesBitmapData[nind].data[Xind] == shadesBitmapData[nind2].data[Xind])
                        if (shadesBitmapData[nind].data[Xind] < 255)
                            shadesBitmapData[nind].data[Xind] = shadesBitmapData[nind].data[Xind] + 1;
                        else {
                            printf("Equals 255, %d\n", currentBlockNo);
                            shadesBitmapData[nind].data[Xind] = 1;//(shadesBitmapData[nind].data[Xind] + 1) % 256;
                        }
                }

            }
            
            // for each blockNo, get XUVW. Then just calculate F(X) and replace where necessary

            for (int nind = 0; nind < n; ++nind) {

                Xind =  (currentBlockNo / shadeXBlockNo) * 2 * shadesInfoHeaders[nind].biWidth + (currentBlockNo % shadeXBlockNo) * 2;
                Vind = ((currentBlockNo / shadeXBlockNo) * 2 + 1) * shadesInfoHeaders[nind].biWidth + (currentBlockNo % shadeXBlockNo) * 2;
                Uind = Vind + 1;
                Wind = Xind + 1;

                // check if two Xs are the same

                // Now, evaluate polynomial in Xind
                F_X = 0x0000;
                for (int pow = 0; pow < k; pow++) {
                    F_X ^= multiplyModGenP((uint16_t) currentBlockStart[pow], galoisPower((uint16_t) shadesBitmapData[nind].data[Xind], pow) );
                }
                FxByte = (uint8_t) F_X;
            
                // Having F_X, change values for U, V and W

                // first calculate them here for clarity
                newW = (shadesBitmapData[nind].data[Wind] & 0b11111000) | ((FxByte & 0b11100000)>>5); // Three highest bits of FX in 3 lowest bits of ~W
                newV = (shadesBitmapData[nind].data[Vind] & 0b11111000) | ((FxByte & 0b00011100)>>2); // Second group of 3 highest bits of FX in 3 lower bits of ~V
                newU = (shadesBitmapData[nind].data[Uind] & 0b11111000) | (FxByte & 0b00000011) | (calcParityBit(FxByte) << 2); // 2 LB of FX in 2 LB of ~U but also parity in 3rd lowest bit of ~U
            
                // then write them off
                shadesBitmapData[nind].data[Wind] = newW;
                shadesBitmapData[nind].data[Vind] = newV;
                shadesBitmapData[nind].data[Uind] = newU;

                currentEvaluatedFs[nind] = FxByte;
            }

            // TODO check if fs are OK

            currentBlockStart += k;
            // printf("Progress: %g\n", currentBlockNo/(double)secretBlockCount);
        }

        // save the images to see if it works
        for (int nind = 0; nind < n; nind++) {
            turnRowsUpsideDown(shadesBitmapData[nind].data, shadesInfoHeaders[nind]);
            writeBitmapToFile(shadesPathList[nind], &shadesFileHeaders[nind], &shadesInfoHeaders[nind], shadesBitmapData[nind]);
        }


    return 0;
}