#ifndef IO_H
#define IO_H
#include "image.h"
#include "gif.h"
#include <fstream>
#include <iostream>


class IO
{
    void decompress(int img);
    char *m_fileContent;
    std::string m_fileName;
    int gce, par, pte, appEx, commEx;
    char *colorTable;
    Image* images;
    Gif gif;
    void setHeader(char* output, int& pointer);
    void getScreen(int& pointer);
    void setScreen(char* output, int& pointer);
    void getLCT(int &pointer, int img);
    void setLCT(char* output, int& pointer, int img);
    void getGCT(int &pointer);
    void setGCT(char* output, int& pointer);
    void getGCE(int &pointer, int img);
    void setGCE(char* output, int& pointer, int img);
    void getIDiscr(int &pointer, int img);
    void setIDiscr(char* output, int& pointer, int img);
    void getIData(int &pointer, int img);
    void setIData(char* output, int& pointer, int img);
    void getPTE(int &pointer);
    void getAE(int &pointer);
    void getCE(int &pointer);
    void getTrailer(int &pointer);
    void setTrailer(char* output, int& pointer);
    unsigned int getNextByte(int &pointer);
    void getFile(char *m_fileName, char *s, int n);
    void saveFile(char *m_fileName, char *output, int fileSize);

public:
    IO();
    IO(std::string p_filePath);
    void loadFile();
    void generateFile();
    Gif* getGif();
    static int getBit(int wert, int bit, int anz);
    static int zweiHochX(int x);
    static char getHex(int i);
    static unsigned char getBinChar(unsigned int b);
};

#endif // IO_H
