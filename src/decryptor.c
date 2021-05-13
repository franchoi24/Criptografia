#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "../include/bmpProcessing.h"
#include "../include/utils.h"


int main (int argc, char * argv[]) {

    int k = 5;
    char shadesDirName[512] = "res/shades";
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
    }

    int secretImageSize = shadesInfoHeaders[0].biWitdh * shadesInfoHeaders[0].biHeight;
    int secretBlockCount = secretImageSize / k;

    uint8_t * secretImageData = (uint8_t) * malloc(secretImageSize);

    uint8_t currentTs[k] = {0};
    uint8_t currentXs[k] = {0};
    uint8_t currentSecretBlock[k] = {0};
    // uint8_t * secretBlockPosition = secretImageData;
    uint8_t T;
    int fail = 0;
    for (int currentBlockNo = 0; currentBlockNo < secretBlockCount; ++currentBlockNo) {
        
        for (int kind = 0; kind < k & !fail; kind ++) {
            // For each shade, get T
            Xind =  (currentBlockNo / shadeXBlockNo) * 2 * shadesInfoHeaders[k].biWidth + (currentBlockNo % shadeXBlockNo) * 2;
            Vind = ((currentBlockNo / shadeXBlockNo) * 2 + 1) * shadesInfoHeaders[k].biWidth + (currentBlockNo % shadeXBlockNo) * 2;
            Uind = Vind + 1;
            Wind = Xind + 1;

            T = ((shadesBitmapData[k].data[Wind] & 0b00000111) << 5) |
                    ((shadesBitmapData[k].data[Vind] & 0b00000111) << 2) |
                        ((shadesBitmapData[k].data[Uind] & 0b00000011)) |

            // Check parity bit
            if (((shadesBitmapData[k].data[Uind] & 0b00000100) >> 2) != calcParityBit(T))
                fail = 1;

            currentTs[kind] = T;
            // For each shade, get X
            currentXs[kind] = shadesBitmapData[k].data[Xind];
        }

        // TODO react to fail in a better way.
        // if (fail) {
        //     fail = 0;
        //     continue;
        // }

        // With (X, T) pairs, interpolate using Lagrange
        lagrangeInterpolate(currentSecretBlock, currentXs, currentYs, k);

        // copy secret block to solution
        for (int kind = 0; kind < k; ++kind) {
            secretImageData[currentBlockNo * k + kind] = currentSecretBlock[kind];
        }


        printf("Progress: %g\n", currentBlockNo/(double)secretBlockCount);
    }

    return 0;
}