#include "interlacedpicture.h"

InterlacedPicture::InterlacedPicture()
{
}

unsigned char *InterlacedPicture::getUninterlacedPicture(unsigned char *pixel, int width, int height)
{
    unsigned char *uninterlaced = new unsigned char[width*height*3];
    int k = 0;
    for(int j = 0; j<height; j+=8){
        for(int i = 0; i<width*3; ++i){
            uninterlaced[j*width*3+i] = pixel[k++];
        }
    }
    for(int j = 4; j<height; j+=8){
        for(int i = 0; i<width*3; ++i){
            uninterlaced[j*width*3+i] = pixel[k++];
        }
    }
    for(int j = 2; j<height; j+=4){
        for(int i = 0; i<width*3; ++i){
            uninterlaced[j*width*3+i] = pixel[k++];
        }
    }
    for(int j = 1; j<height; j+=2){
        for(int i = 0; i<width*3; ++i){
            uninterlaced[j*width*3+i] = pixel[k++];
        }
    }
    return uninterlaced;
}
