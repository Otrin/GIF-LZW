#ifndef GIF_H
#define GIF_H
#include "image.h"
#include "picture.h"
#include <fstream>
#include <iostream>

class Gif : public Picture
{
    int delayTime, gctFlag, sizeOfGCT, bgColor, sizeOfImages;
    Image* images;
    char* colorTable;
    char* codeTable;
public:
    Gif();
    Image *getImages() const;
    void setImages(Image *value);
    Image getImage(int img);
    int getDelayTime() const;
    void setDelayTime(int value);
    char *getColorTable() const;
    void setColorTable(char *value);
    char *getCodeTable() const;
    void setCodeTable(char *value, int n);
    int getGctFlag() const;
    void setGctFlag(int value);
    int getSizeOfGCT() const;
    void setSizeOfGCT(int value);
    int getBgColor() const;
    void setBgColor(int value);
    int getSizeOfImages() const;
    void setSizeOfImages(int value);
    void extendImages(int n);
};

#endif // GIF_H
