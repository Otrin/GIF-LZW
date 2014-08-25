#ifndef INTERLACEDPICTURE_H
#define INTERLACEDPICTURE_H

/**
 * @brief
 *
 */
class InterlacedPicture
{
public:
    /**
     * @brief
     *
     */
    InterlacedPicture();
    /**
     * @brief reads a pixelarray and convert it into a normal pixel-array (not interlaced)
     *
     * @param pixel pixel-array
     * @param width width of the picture
     * @param height height of the picture
     * @return unsigned char the converted pixel-array
     */
    static unsigned char *getUninterlacedPicture(unsigned char *pixel, int width, int height);
};

#endif // INTERLACEDPICTURE_H
