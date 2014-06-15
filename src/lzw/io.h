#ifndef IO_H
#define IO_H
#include "image.h"
#include "gif.h"
#include <fstream>
#include <iostream>


class IO
{
    void decompress(int img);
    char *cstring;
    int width, height, gctf, colorRes, sortFlag,
    sizeOfColorTable, bgColor, par, gce, pte, appEx,
    commEx, sizeOfCodeTable;
    char *colorTable;
    Gif gif;
    void setScreen(int& pointer);
    void getLCT(int &pointer, int img);
    void getGCT(int &pointer);
    void getGCE(int &pointer, int img);
    void getIDiscr(int &pointer, int img);
    void getIData(int &pointer, int img);
    void getPTE(int &pointer);
    void getAE(int &pointer);
    void getCE(int &pointer);
    void getTrailer(int &pointer);
    unsigned int getNextByte(int &pointer);
    void getFile(char *s, int n);

public:
    IO();
    IO(char* s);
    Gif getGif();
};

#endif // IO_H
