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
    int m_gctFlag, m_sizeOfGCT, m_bgColor, m_sizeOfFrames, m_ownGCT;
	vector<Frame*> m_frames;
    unsigned char* m_GCT;
    char m_bgRed, m_bgGreen, m_bgBlue;
public:
    Gif();
	Gif(const Gif& p_toCopy);
    ~Gif();
    Frame *getFrame(int frame);
    unsigned char *getGCT() const;
    void setGCT(unsigned char *p_value, int p_size);
    void setGCT(vector<unsigned char> p_value, int p_size);
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
