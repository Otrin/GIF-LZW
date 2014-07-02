#include "gif.h"
#include "image.h"
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

int Gif::getSizeOfImages() const
{
    return m_sizeOfImages;
}

void Gif::extendImages(int p_n)
{
    Image* old = m_images;
    m_images = new Image[m_sizeOfImages+p_n];
    for(int i = 0; i<m_sizeOfImages; ++i){
        m_images[i] = old[i];
    }
    m_sizeOfImages += p_n;
    delete[] old;
}

Gif &Gif::operator=(const Gif &p_toCopy)
{
    if(&p_toCopy != this){
        m_gctFlag = p_toCopy.m_gctFlag;
        m_sizeOfGCT = p_toCopy.m_sizeOfGCT;
        m_bgColor = p_toCopy.m_bgColor;
        m_sizeOfImages = p_toCopy.m_sizeOfImages;
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

        for (int i = 0; i < m_sizeOfImages; i++) {
            m_images[i] = p_toCopy.m_images[i];
        }
    }
}

Gif::Gif(){
    m_sizeOfGCT = 1;
    m_colorTable = new char[1];
    m_sizeOfImages = 1;
    m_images = new Image[1];
}

Gif::~Gif()
{
    delete[] m_colorTable;
    if(m_images != NULL) delete[] m_images;
}

Image *Gif::getImages()
{
    return m_images;
}

Image *Gif::getImage(int img)
{
    return &m_images[img];
}
