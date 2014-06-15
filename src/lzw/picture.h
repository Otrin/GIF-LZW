#ifndef PICTURE_H
#define PICTURE_H

class Picture
{
    int height;
    int width;
    char* pixles;
public:
    Picture(int w, int h, char* p);
    Picture();
    int getHeight() const;
    void setHeight(int value);
    int getWidth() const;
    void setWidth(int value);
    char *getPixles() const;
    void setPixles(char *value);
};

#endif // PICTURE_H
