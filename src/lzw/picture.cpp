#include "picture.h"
#include <iostream>

int Picture::getHeight() const
{
    return height;
}

int Picture::getWidth() const
{
    return width;
}

unsigned char *Picture::getPixel() const
{
    return pixel;
}

void Picture::setHeight(int value)
{
    height = value;
}

void Picture::setWidth(int value)
{
    width = value;
}

void Picture::setPixel(unsigned char *value)
{
    delete[] pixel;
    pixel = new unsigned char[width*height*3];
    for(int i = 0; i<width*height*3; ++i){
        pixel[i] = value[i];
    }
}

Picture &Picture::operator=(const Picture &p_toCopy)
{
    if(&p_toCopy != this){
        width = p_toCopy.width;
        height = p_toCopy.height;
        delete[] pixel;
        pixel = new unsigned char[height*width*3];
        for(int i = 0; i<height*width*3; ++i){
            pixel[i] = p_toCopy.pixel[i];
        }
    }
    return *this;
}
Picture::Picture(int w, int h, unsigned char *p)
{
    this->width = w;
    this->height = h;
    this->pixel = p;
}

Picture::Picture()
{
    pixel = NULL;
    height = width = 0;
}

Picture::~Picture()
{
    //delete[] pixel;
}
