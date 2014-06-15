#include "image.h"


int Image::getTop() const
{
    return top;
}

void Image::setTop(int value)
{
    top = value;
}

int Image::getLeft() const
{
    return left;
}

void Image::setLeft(int value)
{
    left = value;
}

int Image::getHeight() const
{
    return height;
}

void Image::setHeight(int value)
{
    height = value;
}

int Image::getDelayTime() const
{
    return delayTime;
}

void Image::setDelayTime(int value)
{
    delayTime = value;
}

int Image::getTranspColorIndex() const
{
    return transpColorIndex;
}

void Image::setTranspColorIndex(int value)
{
    transpColorIndex = value;
}

int Image::getTranspColorFlag() const
{
    return transpColorFlag;
}

void Image::setTranspColorFlag(int value)
{
    transpColorFlag = value;
}

int Image::getLctFlag() const
{
    return lctFlag;
}

void Image::setLctFlag(int value)
{
    lctFlag = value;
}

int Image::getSortFlag() const
{
    return sortFlag;
}

void Image::setSortFlag(int value)
{
    sortFlag = value;
}

int Image::getSizeOfLCT() const
{
    return sizeOfLCT;
}

void Image::setSizeOfLCT(int value)
{
    sizeOfLCT = value;
}

char *Image::getLct() const
{
    return lct;
}

void Image::setLct(char *value)
{
    lct = value;
}

unsigned char *Image::getCodeTable()
{
    return codeTable;
}

void Image::setCodeTable(unsigned char *value)
{
    codeTable = value;
}

int Image::getWidth() const
{
    return width;
}

void Image::setWidth(int value)
{
    width = value;
}

int Image::getMinCodeSize() const
{
    return minCodeSize;
}

void Image::setMinCodeSize(int value)
{
    minCodeSize = value;
}

int Image::getSizeOfCodeTable() const
{
    return sizeOfCodeTable;
}

void Image::setSizeOfCodeTable(int value)
{
    sizeOfCodeTable = value;
}

char *Image::getPixles() const
{
    return pixles;
}

void Image::setPixles(char *value)
{
    pixles = value;
}
Image::Image()
{
}
