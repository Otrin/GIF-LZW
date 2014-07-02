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

void Image::setLct(char *value, int size)
{
    sizeOfLCT = size;
    delete[] lct;
    lct = new char[sizeOfLCT];
    for(int i = 0; i<sizeOfLCT; ++i){
        lct[i] = value[i];
    }
}

unsigned char *Image::getCodeTable()
{
    return codeTable;
}

void Image::setCodeTable(unsigned char *value, int size)
{
    sizeOfCodeTable = size;
    delete[] codeTable;
    codeTable = new unsigned char[sizeOfCodeTable];
    for(int i = 0; i<sizeOfCodeTable; ++i){
        codeTable[i] = value[i];
    }
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
    unsigned char *old = codeTable;
    codeTable = new unsigned char[value];
    for(int i = 0; i<sizeOfCodeTable; ++i){
        codeTable[i] = old[i];
    }
    delete[] old;
    sizeOfCodeTable = value;
}

char *Image::getPixel() const
{
    return pixel;
}

void Image::setPixel(char *value, int size)
{
    sizeOfPixel = size;
    delete[] pixel;
    pixel = new char[sizeOfPixel];
    for(int i = 0; i<sizeOfPixel; ++i){
        pixel[i] = value[i];
    }
}


int Image::getSizeOfPixel() const
{
    return sizeOfPixel;
}

void Image::setSizeOfPixel(int value)
{
    sizeOfPixel = value;
}

Image &Image::operator=(const Image &toCopy)
{
    if(&toCopy != this){
        left = toCopy.left;
        top = toCopy.top;
        width = toCopy.width;
        height = toCopy.height;
        delayTime = toCopy.delayTime;
        transpColorIndex = toCopy.transpColorIndex;
        transpColorFlag = toCopy.transpColorFlag;
        lctFlag = toCopy.lctFlag;
        sortFlag = toCopy.sortFlag;
        sizeOfLCT = toCopy.sizeOfLCT;
        sizeOfCodeTable = toCopy.sizeOfCodeTable;
        sizeOfPixel = toCopy.sizeOfPixel;
        minCodeSize = toCopy.minCodeSize;

        lct = new char[sizeOfLCT];
        codeTable = new unsigned char[sizeOfCodeTable];
        pixel = new char[sizeOfPixel];

        for (int i = 0; i < sizeOfLCT; ++i) {
            lct[i] = toCopy.lct[i];
        }


        for (int i = 0; i < sizeOfCodeTable; ++i) {
            codeTable[i] = toCopy.codeTable[i];
        }

        for (int i = 0; i < sizeOfPixel; ++i) {
            pixel[i] = toCopy.pixel[i];
        }
    }
    return *this;
}


Image::Image()
{
    codeTable = new unsigned char[1];
    sizeOfCodeTable = 1;
    lct = new char[1];
    sizeOfLCT = 1;
    pixel = new char[1];
    sizeOfPixel = 1;
    delayTime = 0;
    transpColorIndex = 0;
    sortFlag = 0;
    top = 0;
    transpColorFlag = 0;
    left = 0;
    width = 0;
    height = 0;
    lctFlag = 0;
    minCodeSize = 0;
}

Image::~Image()
{
    delete[] pixel;
    delete[] lct;
    delete[] codeTable;
}
