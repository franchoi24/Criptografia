#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include "../include/bmpProcessing.h"

// https://stackoverflow.com/questions/14279242/read-bitmap-file-into-structure

uint8_t *LoadBitmapFile(char *filename, BITMAPFILEHEADER * bitmapFileHeader, BITMAPINFOHEADER *bitmapInfoHeader)
{
    FILE *filePtr; //our file pointer
    // BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
    uint8_t *bitmapImage;  //store image data
    int imageIdx=0;  //image index counter
    uint8_t tempRGB;  //our swap variable

    //open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return NULL;


    //read the bitmap file header
    fread(bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader->bfType !=0x4D42)
    {
        fclose(filePtr);
        return NULL;
    }


    //read the bitmap info header
    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr); 

    //move file point to the beginning of bitmap data
    fseek(filePtr, bitmapFileHeader->bfOffBits, SEEK_SET);

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
        return NULL;
    }

    //read in the bitmap image data
    fread(bitmapImage,bitmapInfoHeader->biSizeImage,1,filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return NULL;
    }

    //swap the r and b values to get RGB (bitmap is BGR)
    for (imageIdx = 0;imageIdx < bitmapInfoHeader->biSizeImage;imageIdx+=3)
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    //close file and return bitmap iamge data
    fclose(filePtr);
    return bitmapImage;
}

void writeBitmapToFile(char * filename, BITMAPFILEHEADER * bitmapFileHeader, BITMAPINFOHEADER *bitmapInfoHeader, uint8_t * bitmapImage) {
    FILE * fd = fopen("res/newImg.bmp", "w");
    fwrite(bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fd);
    fwrite(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, fd);

    //swap the r and b values to get RGB (bitmap is BGR)
    uint8_t tempRGB;
    for (int imageIdx = 0;imageIdx < bitmapInfoHeader->biSizeImage;imageIdx+=3)
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    fwrite(bitmapImage, bitmapInfoHeader->biSizeImage, 1, fd);
    fclose(fd);
}