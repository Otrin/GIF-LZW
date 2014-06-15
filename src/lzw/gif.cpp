#include "gif.h"
#include "image.h"


char *Gif::getColorTable() const
{
    return colorTable;
}

void Gif::setColorTable(char *value)
{
    colorTable = value;
}

char *Gif::getCodeTable() const
{
    return codeTable;
}

void Gif::setCodeTable(char *value, int n)
{
    codeTable = new char[n];
    for(int i = 0; i<n; ++i){
        codeTable[i] = value[i];
    }
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

void Gif::setSizeOfImages(int value)
{
    sizeOfImages = value;
}

void Gif::extendImages(int n)
{
    Image* old = images;
    images = new Image[sizeOfImages];
    for(int i = 0; i<sizeOfImages; ++i){
        images[i] = old[i];
    }
    sizeOfImages += n;
    delete[] old;
}
Gif::Gif(){

}


int Gif::getDelayTime() const
{
    return delayTime;
}

void Gif::setDelayTime(int value)
{
    delayTime = value;
}
Image *Gif::getImages() const
{
    return images;
}

void Gif::setImages(Image *value)
{
    images = value;
}

Image Gif::getImage(int img)
{
    return images[img];
}
