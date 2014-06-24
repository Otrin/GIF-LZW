#include "gif.h"
#include "image.h"
#include <iostream>

using namespace std;


char *Gif::getColorTable() const
{
    return colorTable;
}

void Gif::setColorTable(char *value, int size)
{
    colorTable = value;
}

int Gif::getGctFlag() const
{
    return gctFlag;
}

void Gif::setGctFlag(int value)
{
    gctFlag = value;
}

int Gif::getSizeOfGCT() const
{
    return sizeOfGCT;
}

void Gif::setSizeOfGCT(int value)
{
    sizeOfGCT = value;
}

int Gif::getBgColor() const
{
    return bgColor;
}

void Gif::setBgColor(int value)
{
    bgColor = value;
}

int Gif::getSizeOfImages() const
{
    return sizeOfImages;
}

void Gif::extendImages(int n)
{
    Image* old = images;
    images = new Image[sizeOfImages+n];
    for(int i = 0; i<sizeOfImages; ++i){
        images[i] = old[i];
    }
    sizeOfImages += n;
    delete[] old;
}

Gif &Gif::operator=(const Gif &toCopy)
{
    if(&toCopy != this){
        delayTime = toCopy.delayTime;
        gctFlag = toCopy.gctFlag;
        sizeOfGCT = toCopy.sizeOfGCT;
        bgColor = toCopy.bgColor;
        sizeOfImages = toCopy.sizeOfImages;
        height = toCopy.height;
        width = toCopy.width;

        pixel = new char[(height*width*3)];
        for (int i = 0; i < (height*width*3); i++) {
            pixel[i] = toCopy.pixel[i];
        }

        colorTable = new char[sizeOfGCT];
        for (int i = 0; i < sizeOfGCT; i++) {
            colorTable[i] = toCopy.colorTable[i];
        }

        for (int i = 0; i < sizeOfImages; i++) {
            images[i] = toCopy.images[i];
        }
    }
}


Gif::Gif(){
    sizeOfGCT = 1;
    colorTable = new char[1];
    sizeOfImages = 1;
    images = new Image[1];
}

Gif::~Gif()
{
    delete[] colorTable;
    if(images != NULL) delete[] images;
}


int Gif::getDelayTime() const
{
    return delayTime;
}

void Gif::setDelayTime(int value)
{
    delayTime = value;
}
Image *Gif::getImages()
{
    return images;
}

Image *Gif::getImage(int img)
{
    return &images[img];
}
