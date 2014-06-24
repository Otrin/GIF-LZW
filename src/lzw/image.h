#ifndef IMAGE_H
#define IMAGE_H

class Image
{
int left, top, width, height, delayTime, transpColorIndex,
transpColorFlag, lctFlag, sortFlag, sizeOfLCT, sizeOfCodeTable, sizeOfPixel, minCodeSize;
char* lct;
unsigned char* codeTable;
char* pixel;
public:
    Image();
    ~Image();
    int getTop() const;
    void setTop(int value);
    int getLeft() const;
    void setLeft(int value);
    int getHeight() const;
    void setHeight(int value);
    int getDelayTime() const;
    void setDelayTime(int value);
    int getTranspColorIndex() const;
    void setTranspColorIndex(int value);
    int getTranspColorFlag() const;
    void setTranspColorFlag(int value);
    int getLctFlag() const;
    void setLctFlag(int value);
    int getSortFlag() const;
    void setSortFlag(int value);
    int getSizeOfLCT() const;
    void setSizeOfLCT(int value);
    char *getLct() const;
    void setLct(char *value, int size);
    unsigned char *getCodeTable();
    void setCodeTable(unsigned char *value, int size);
    int getWidth() const;
    void setWidth(int value);
    int getMinCodeSize() const;
    void setMinCodeSize(int value);
    int getSizeOfCodeTable() const;
    void setSizeOfCodeTable(int value);
    char *getPixel() const;
    void setPixel(char *value, int size);
    int getSizeOfPixel() const;
    void setSizeOfPixel(int value);
    Image& operator=(const Image& toCopy);
};

#endif // IMAGE_H
