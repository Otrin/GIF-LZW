#ifndef GIF_H
#define GIF_H
#include <fstream>
#include <iostream>

class Gif
{
public:
    char *cstring;
    int width, height, gctf, colorRes, sortFlag, sizeOfColorTable, bgColor, par, gce, pte, appEx, commEx, delayTime, lctFlag, localColorTable, sizeOfCodeTable;
    char *colorTable;
    unsigned char *codeTable;
    void getScreen(int& pointer);
    void getCT(int count, int &pointer, char *&table);
    void getGCE(int &pointer);
    void getIDiscr(int &pointer);
    void getLCT(int &pointer);
    void getIData(int &pointer);
    void getPTE(int &pointer);
    void getAE(int &pointer);
    void getCE(int &pointer);
    void getTrailer(int &pointer);
    unsigned int getNext(int &pointer);
    Gif(char* s);

};

#endif // GIF_H
