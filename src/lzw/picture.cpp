#include "picture.h"


int Picture::getHeight() const
{
    return height;
}

int Picture::getWidth() const
{
    return width;
}

char *Picture::getPixel() const
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

void Picture::setPixel(char *value)
{
    pixel = value;
}
Picture::Picture(int w, int h, char *p)
{
    this->width = w;
    this->height = h;
    this->pixel = p;
}

Picture::Picture()
{

}
