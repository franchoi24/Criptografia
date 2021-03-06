#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include "../include/bmpProcessing.h"

// A modification of LoadBitmapFile from https://stackoverflow.com/questions/14279242/read-bitmap-file-into-structure

BITMAPDATA LoadBitmapFile(char *filename, BITMAPFILEHEADER * bitmapFileHeader, BITMAPINFOHEADER *bitmapInfoHeader)
{
    FILE *filePtr; //our file pointer
    // BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
    uint8_t *bitmapImage;  //store image data

    BITMAPDATA bitmapData = {0};

    //open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return bitmapData;


    //read the bitmap file header
    fread(bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader->bfType !=0x4D42)
    {
        fclose(filePtr);
        return bitmapData;
    }


    //read the bitmap info header
    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr); 

    int paddingSize = bitmapFileHeader->bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);

    uint8_t * padding = (uint8_t *) malloc(paddingSize);
    fread(padding, paddingSize, 1, filePtr);
    //move file point to the beginning of bitmap data
    // fseek(filePtr, bitmapFileHeader->bfOffBits, SEEK_SET);

    // weird stuff. Sometimes biSizeImage is not present, but width and height are
    if (bitmapInfoHeader->biSizeImage == 0) {
        bitmapInfoHeader->biSizeImage = bitmapInfoHeader->biWidth * bitmapInfoHeader->biHeight;
    }

    //allocate enough memory for the bitmap image data
    bitmapImage = (uint8_t*)malloc(bitmapInfoHeader->biSizeImage);

    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        return bitmapData;
    }

    //read in the bitmap image data
    fread(bitmapImage,bitmapInfoHeader->biSizeImage,1,filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return bitmapData;
    }

    //close file and return bitmap iamge data
    fclose(filePtr);
    bitmapData.paddingSize = paddingSize;
    bitmapData.padding = padding;
    bitmapData.data = bitmapImage;
    return bitmapData;
}

void writeBitmapToFile(char * filename, BITMAPFILEHEADER * bitmapFileHeader, BITMAPINFOHEADER *bitmapInfoHeader, BITMAPDATA bitmapImage) {
    FILE * fd = fopen(filename, "w");
    fwrite(bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fd);
    fwrite(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, fd);


    fwrite(bitmapImage.padding, bitmapImage.paddingSize, 1, fd);
    fwrite(bitmapImage.data, bitmapInfoHeader->biSizeImage, 1, fd);
    fclose(fd);
}

void turnRowsUpsideDown(uint8_t * bitmapData, BITMAPINFOHEADER infoHeader) {
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

void freeBitmapData(BITMAPDATA bmpData) {
    free(bmpData.data);
    free(bmpData.padding);
}
