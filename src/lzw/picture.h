#ifndef PICTURE_H
#define PICTURE_H

class Picture
{
    int height;
    int width;
    char* pixel;
public:
    Picture(int w, int h, char* p);
    Picture();
    int getHeight() const;
    void setHeight(int value);
    int getWidth() const;
    void setWidth(int value);
    char *getPixel() const;
    void setPixel(char *value);
};

#endif // PICTURE_H
