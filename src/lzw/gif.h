#ifndef GIF_H
#define GIF_H
#include "frame.h"
#include "picture.h"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class Gif : public Picture
{
    int m_gctFlag, m_sizeOfGCT, m_bgColor, m_sizeOfFrames;
    vector<Frame> m_frames;
    char* m_colorTable;
public:
    Gif();
    ~Gif();
    Frame *getFrame(int frame);
    char *getColorTable() const;
    void setColorTable(char *p_value, int p_size);
    int getGctFlag() const;
    void setGctFlag(int p_value);
    int getSizeOfGCT() const;
    void setSizeOfGCT(int p_value);
    int getBgColor() const;
    void setBgColor(int p_value);
    int getSizeOfFrames() const;
    void extendFrames();
    Gif& operator=(const Gif& p_toCopy);
};

#endif // GIF_H
