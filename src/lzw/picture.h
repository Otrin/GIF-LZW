#ifndef PICTURE_H
#define PICTURE_H

class Picture
{
protected:
    int height;
    int width;
    unsigned char* pixel;
public:
    Picture(int w, int h, unsigned char *p);
    Picture();
    ~Picture();
    int getHeight() const;
    void setHeight(int value);
    int getWidth() const;
    void setWidth(int value);
    unsigned char *getPixel() const;
    void setPixel(unsigned char *value);
    Picture& operator=(const Picture& p_toCopy);
};

#endif // PICTURE_H
