#ifndef INTERLACEDPICTURE_H
#define INTERLACEDPICTURE_H

class InterlacedPicture
{
public:
    InterlacedPicture();
    static char* getUninterlacedPicture(char *pixel, int width, int height);
};

#endif // INTERLACEDPICTURE_H
