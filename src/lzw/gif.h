#ifndef GIF_H
#define GIF_H
#include "frame.h"
#include "picture.h"
#include <fstream>
#include <iostream>

class Gif : public Picture
{
    int m_gctFlag, m_sizeOfGCT, m_bgColor, m_sizeOfImages;
    Frame* m_frames;
    char* m_colorTable;
public:
    Gif();
    ~Gif();
    Frame *getFrames();
    Frame *geFrame(int frame);
    int getDelayTime() const;
    void setDelayTime(int value);
    char *getColorTable() const;
    void setColorTable(char *p_value, int p_size);
    int getGctFlag() const;
    void setGctFlag(int p_value);
    int getSizeOfGCT() const;
    void setSizeOfGCT(int p_value);
    int getBgColor() const;
    void setBgColor(int p_value);
    int getSizeOfImages() const;
    void extendImages(int p_n);
    Gif& operator=(const Gif& p_toCopy);
};

#endif // GIF_H
