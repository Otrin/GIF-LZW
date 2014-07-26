#ifndef INTERLACEDPICTURE_H
#define INTERLACEDPICTURE_H

class InterlacedPicture
{
public:
    InterlacedPicture();
    static unsigned char *getUninterlacedPicture(unsigned char *pixel, int width, int height);
};

#endif // INTERLACEDPICTURE_H
