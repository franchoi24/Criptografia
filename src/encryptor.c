#include <stdlib.h>
#include <stdio.h>
#include "../include/bmpProcessing.h"

int main (int argc, char *argv[]) {
    BITMAPINFOHEADER secretBmpIH = {0};
    uint8_t *secretBitmapData;
    FILE *fp;
    secretBitmapData = LoadBitmapFile("res/Audrey.bmp",&secretBmpIH);
    fp = fopen("res/example.txt", "w");
    fwrite(secretBitmapData,1, sizeof(secretBitmapData), fp);
    fclose(fp);

    int k = 5;
    int secretBlockCount = secretBmpIH.biSizeImage / k;

    for (uint8_t * currentBlock; (secretBitmapData - currentBlock) / k < secretBlockCount; currentBlock += k) {

    }

    return 0;
}