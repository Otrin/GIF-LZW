#ifndef PICTURE_H
#define PICTURE_H

class Picture
{
protected:
    int height;
    int width;
    char* pixel;
public:
    Picture(int w, int h, char* p);
    Picture();
    ~Picture();
    int getHeight() const;
    void setHeight(int value);
    int getWidth() const;
    void setWidth(int value);
    char *getPixel() const;
    void setPixel(char *value);
    Picture& operator=(const Picture& p_toCopy);
};

#endif // PICTURE_H
