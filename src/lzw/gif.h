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
public:
    Gif();
    ~Gif();
    Image *getImages();
    Image *getImage(int img);
    int getDelayTime() const;
    void setDelayTime(int value);
    char *getColorTable() const;
    void setColorTable(char *value, int size);
    int getGctFlag() const;
    void setGctFlag(int value);
    int getSizeOfGCT() const;
    void setSizeOfGCT(int value);
    int getBgColor() const;
    void setBgColor(int value);
    int getSizeOfImages() const;
    void extendImages(int n);
};

#endif // GIF_H
