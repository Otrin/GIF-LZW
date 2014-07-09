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
    m_sizeOfGCT = p_size;
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

void Gif::extendFrames()
{
    Frame *f = new Frame();
    m_frames.push_back(*f);
    m_sizeOfFrames++;
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
        delete[] pixel;
        pixel = new char[(height*width*3)];
        for (int i = 0; i < (height*width*3); i++) {
            pixel[i] = p_toCopy.pixel[i];
        }
        delete[] m_colorTable;
        m_colorTable = new char[m_sizeOfGCT*3];
        for (int i = 0; i < m_sizeOfGCT*3; i++) {
            m_colorTable[i] = p_toCopy.m_colorTable[i];
        }
        m_frames = p_toCopy.m_frames;
    }
    return *this;
}

Gif::Gif(){
    m_sizeOfGCT = 0;
    m_colorTable = NULL;
    m_sizeOfFrames = 0;
    m_bgColor = 0;
    m_gctFlag = 0;
}

Gif::~Gif()
{
    //delete[] m_colorTable;
}


Frame *Gif::getFrame(int frame)
{
    return &(m_frames.at(frame));
}
