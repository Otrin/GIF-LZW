#ifndef IMAGE_H
#define IMAGE_H

class Image
{
int left, top, width, height, delayTime, transpColorIndex,
transpColorFlag, lctFlag, sortFlag, sizeOfLCT, sizeOfCodeTable, minCodeSize;
char* lct;
unsigned char* codeTable;
char* pixles;
public:
    Image();
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
    void setLct(char *value);
    unsigned char *getCodeTable();
    void setCodeTable(unsigned char *value);
    int getWidth() const;
    void setWidth(int value);
    int getMinCodeSize() const;
    void setMinCodeSize(int value);
    int getSizeOfCodeTable() const;
    void setSizeOfCodeTable(int value);
    char *getPixles() const;
    void setPixles(char *value);
};

#endif // IMAGE_H
