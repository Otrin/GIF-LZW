#include "gif.h"
#include "frame.h"
#include <iostream>

using namespace std;


char *Gif::getColorTable() const
{
    return m_colorTable;
}

void Gif::setColorTable(char *p_value, int p_size)
{
    m_colorTable = p_value;
}

int Gif::getGctFlag() const
{
    return m_gctFlag;
}

void Gif::setGctFlag(int p_value)
{
    m_gctFlag = p_value;
}

int Gif::getSizeOfGCT() const
{
    return m_sizeOfGCT;
}

void Gif::setSizeOfGCT(int p_value)
{
    m_sizeOfGCT = p_value;
}

int Gif::getBgColor() const
{
    return m_bgColor;
}

void Gif::setBgColor(int p_value)
{
    m_bgColor = p_value;
}

int Gif::getSizeOfFrames() const
{
    return m_sizeOfFrames;
}

void Gif::extendImages(int p_n)
{
    Frame* old = m_frames;
    m_frames = new Frame[m_sizeOfFrames+p_n];
    for(int i = 0; i<m_sizeOfFrames; ++i){
        m_frames[i] = old[i];
    }
    m_sizeOfFrames += p_n;
    delete[] old;
}

Gif &Gif::operator=(const Gif &p_toCopy)
{
    if(&p_toCopy != this){
        m_gctFlag = p_toCopy.m_gctFlag;
        m_sizeOfGCT = p_toCopy.m_sizeOfGCT;
        m_bgColor = p_toCopy.m_bgColor;
        m_sizeOfFrames = p_toCopy.m_sizeOfFrames;
        height = p_toCopy.height;
        width = p_toCopy.width;

        pixel = new char[(height*width*3)];
        for (int i = 0; i < (height*width*3); i++) {
            pixel[i] = p_toCopy.pixel[i];
        }

        m_colorTable = new char[m_sizeOfGCT];
        for (int i = 0; i < m_sizeOfGCT; i++) {
            m_colorTable[i] = p_toCopy.m_colorTable[i];
        }

        for (int i = 0; i < m_sizeOfFrames; i++) {
            m_frames[i] = p_toCopy.m_frames[i];
        }
    }
    return *this;
}

Gif::Gif(){
    m_sizeOfGCT = 1;
    m_colorTable = new char[1];
    m_sizeOfFrames = 1;
    m_frames = new Frame[1];
}

Gif::~Gif()
{
    delete[] m_colorTable;
    if(m_frames != NULL) delete[] m_frames;
}

Frame *Gif::getFrames()
{
    return m_frames;
}

Frame *Gif::getFrame(int frame)
{
    return &m_frames[frame];
}
