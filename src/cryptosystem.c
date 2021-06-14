#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "../include/bmpProcessing.h"
#include "../include/galoisFields.h"
#include "../include/utils.h"
#include "../include/cryptosystem.h"

static int getNumberOfBmps(char * shadesDirName) {
    int n = 0;
    
    DIR * d;
    struct dirent * dir;
    d = opendir(shadesDirName);
    if (d) {
        char filePath[600];
        strcpy(filePath, shadesDirName);
        int index = 0;
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".bmp") > 0) {
                n+=1;
            }
        }
        closedir(d);
    }

    return n;
}

int encrypt (char * secretImage, int k, char * dirName) {
    BITMAPINFOHEADER secretBmpIH = {0};
    BITMAPFILEHEADER secretBmpFH = {0};
    BITMAPDATA secretBitmapData;
    secretBitmapData = LoadBitmapFile("res/Albert.bmp",&secretBmpFH,&secretBmpIH);

    int n = getNumberOfBmps(dirName);
    if (n < k) {
        printf("Error: Not enough images to distribute secret!\n");
        exit(1);
    }
    int secretBlockCount = secretBmpIH.biSizeImage / k;

    char shadesDirName[512], outputDirName[512];
    strcpy(shadesDirName, dirName);
    strcpy(outputDirName, dirName);

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
        char filePath[600 + 6]; // 6 == length of "-shade", dynamic memory allocation is a waste of time here
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

        int repeated = 0;

        for (int currentBlockNo = 0; currentBlockNo < secretBlockCount; ++currentBlockNo) {
            
            // Correct for all different Xs

            for (int nind = 0; nind < n; ++nind) {
                Xind =  (currentBlockNo / shadeXBlockNo) * 2 * shadesInfoHeaders[nind].biWidth + (currentBlockNo % shadeXBlockNo) * 2;
                do {
                    repeated = 0;
                    for (int nind2 = 0; nind2 < nind; ++nind2) {
                        // Clearly separate cases of conflict
                        if(nind != nind2 && shadesBitmapData[nind2].data[Xind] == shadesBitmapData[nind].data[Xind]) {
                            repeated = 1;
                            // if (shadesBitmapData[nind].data[Xind] < 255)
                                shadesBitmapData[nind].data[Xind] = (shadesBitmapData[nind].data[Xind] + 1) % 256;
                            // else {
                            //     // printf("Equals 255, %d\n", currentBlockNo);
                            //     shadesBitmapData[nind].data[Xind] = 1;//(shadesBitmapData[nind].data[Xind] + 1) % 256;
                            // }
                        }
                    }
                } while (repeated);

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

            currentBlockStart += k;
            // printf("Progress: %g\n", currentBlockNo/(double)secretBlockCount);
        }

        // save the images to see if it works
        for (int nind = 0; nind < n; nind++) {
            turnRowsUpsideDown(shadesBitmapData[nind].data, shadesInfoHeaders[nind]);
            strcpy(shadesPathList[nind] + strlen(shadesPathList[nind]) - 4, "-shade.bmp");
            writeBitmapToFile(shadesPathList[nind], &shadesFileHeaders[nind], &shadesInfoHeaders[nind], shadesBitmapData[nind]);
            freeBitmapData(shadesBitmapData[nind]);
        }

    return 0;
}


int decrypt (char * secretImage, int k, char * dirName) {

    char shadesDirName[512];
    strcpy(shadesDirName, dirName);
    if (shadesDirName[strlen(shadesDirName)-1] != '/') {
        strcpy(shadesDirName+strlen(shadesDirName), "/");
    }
    // Gather all necessary shades
    BITMAPINFOHEADER shadesInfoHeaders[k];
    BITMAPFILEHEADER shadesFileHeaders[k];
    BITMAPDATA shadesBitmapData[k];

    // To do that, just iterate through shades
    DIR * d;
    struct dirent * dir;
    d = opendir(shadesDirName);
    if (d) {
        char filePath[600];
        strcpy(filePath, shadesDirName);
        int index = 0;
        while ((dir = readdir(d)) != NULL && index < k) {
            if (strstr(dir->d_name, ".bmp") > 0) {
                strcpy(filePath + strlen(shadesDirName), dir->d_name);
                shadesBitmapData[index] = LoadBitmapFile(filePath, &(shadesFileHeaders[index]), &(shadesInfoHeaders[index]));
                turnRowsUpsideDown(shadesBitmapData[index].data, shadesInfoHeaders[index]);
                index++;
            }
        }
        closedir(d);
        if (index < k) {
            printf("Error: Not enough images to recover secret!\n");
            exit(1);
        }
    }

    long imageSize = shadesInfoHeaders[0].biSizeImage;

    for (int i = 0; i < k; i++) {
        if (shadesInfoHeaders[i].biSizeImage != imageSize) {
            printf("Error: Shades not all of the same size!\n");
            exit(1);
        }
    }

    int secretImageSize = shadesInfoHeaders[0].biWidth * shadesInfoHeaders[0].biHeight;
    int secretBlockCount = secretImageSize / k;

    uint8_t * secretImageData = (uint8_t *) malloc(secretImageSize);

    uint8_t currentTs[k];
    uint8_t currentXs[k];
    uint8_t currentSecretBlock[k];
    int Xind, Uind, Vind, Wind;
    int shadeXBlockNo = shadesInfoHeaders[0].biWidth / 2;
    // uint8_t * secretBlockPosition = secretImageData;
    uint8_t T;
    long fail = 0;
    for (int currentBlockNo = 0; currentBlockNo < secretBlockCount; ++currentBlockNo) {
        
        for (int kind = 0; kind < k; kind ++) {
            // For each shade, get T
            Xind =  (currentBlockNo / shadeXBlockNo) * 2 * shadesInfoHeaders[kind].biWidth + (currentBlockNo % shadeXBlockNo) * 2;
            Vind = ((currentBlockNo / shadeXBlockNo) * 2 + 1) * shadesInfoHeaders[kind].biWidth + (currentBlockNo % shadeXBlockNo) * 2;
            Uind = Vind + 1;
            Wind = Xind + 1;

            T = ((shadesBitmapData[kind].data[Wind] & 0b00000111) << 5) |
                    ((shadesBitmapData[kind].data[Vind] & 0b00000111) << 2) |
                        ((shadesBitmapData[kind].data[Uind] & 0b00000011));

            // Check parity bit
            if (((shadesBitmapData[kind].data[Uind] & 0b00000100) >> 2) != calcParityBit(T)){
                // fail = 1
                fail = fail + 1; 
                printf("Block %d: parity failed\n", currentBlockNo);
            }

            currentTs[kind] = T;
            // For each shade, get X
            currentXs[kind] = shadesBitmapData[kind].data[Xind];
        }

        // if (hasEqualBytes(currentXs, k)) {
        //     printf("%p: Repeated values either in Xs, block's gonna fail\n", currentSecretBlock);
        // }

        // With (X, T) pairs, interpolate using Lagrange
        gaussInterpolate(currentSecretBlock, currentXs, currentTs, k);

        // copy secret block to solution
        for (int kind = 0; kind < k; ++kind) {
            secretImageData[currentBlockNo * k + kind] = currentSecretBlock[kind];
        }

        // printf("Progress: %g\n", currentBlockNo/(double)secretBlockCount);
    }

    BITMAPDATA secretBitmapData = {0};
    secretBitmapData.data = secretImageData;
    secretBitmapData.padding = shadesBitmapData[0].padding;
    secretBitmapData.paddingSize = shadesBitmapData[0].paddingSize;
    // printf("%d", secretBlockCount);
    writeBitmapToFile(secretImage, &shadesFileHeaders[0], &shadesInfoHeaders[0], secretBitmapData);

    for (int i = 0; i < k; i++) {
        freeBitmapData(shadesBitmapData[i]);
    }
    return 0;
}