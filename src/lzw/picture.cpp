#include "picture.h"


int Picture::getHeight() const
{
    return height;
}

int Picture::getWidth() const
{
    return width;
}

char *Picture::getPixles() const
{
    return pixles;
}

void Picture::setHeight(int value)
{
    height = value;
}

void Picture::setWidth(int value)
{
    width = value;
}

void Picture::setPixles(char *value)
{
    pixles = value;
}
Picture::Picture(int w, int h, char *p)
{
    this->width = w;
    this->height = h;
    this->pixles = p;
}

Picture::Picture()
{

}
