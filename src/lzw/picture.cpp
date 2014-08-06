#include "picture.h"
#include <iostream>

int Picture::getHeight() const
{
    return m_height;
}

int Picture::getWidth() const
{
    return m_width;
}

unsigned char *Picture::getPixel() const
{
    return pixel;
}

void Picture::setHeight(int value)
{
    m_height = value;
}

void Picture::setWidth(int value)
{
    m_width = value;
}

void Picture::setPixel(unsigned char *value)
{
    delete[] pixel;
    pixel = new unsigned char[m_width*m_height*3];
    for(int i = 0; i<m_width*m_height*3; ++i){
        pixel[i] = value[i];
    }
}

Picture &Picture::operator=(const Picture &p_toCopy)
{
    if(&p_toCopy != this){
        m_width = p_toCopy.m_width;
        m_height = p_toCopy.m_height;
        delete[] pixel;
        pixel = new unsigned char[m_height*m_width*3];
        for(int i = 0; i<m_height*m_width*3; ++i){
            pixel[i] = p_toCopy.pixel[i];
        }
    }
    return *this;
}
Picture::Picture(int w, int h, unsigned char *p)
{
    this->m_width = w;
    this->m_height = h;
    this->pixel = p;
}

Picture::Picture()
{
    pixel = NULL;
    m_height = m_width = 0;
}

Picture::~Picture()
{
    //delete[] pixel;
}
