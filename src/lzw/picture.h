#ifndef PICTURE_H
#define PICTURE_H

/**
 * @brief
 *
 */
class Picture
{
protected:
    int m_height; /**< hight of the picture */
    int m_width; /**< width of the picture */
    unsigned char* pixel; /**< pixel-array */
public:
    /**
     * @brief generates a picture withe the height, the width and the pixel-array
     *
     * @param w width
     * @param h height
     * @param p pixel-array
     */
    Picture(int w, int h, unsigned char *p);
    /**
     * @brief
     *
     */
    Picture();
    /**
     * @brief
     *
     */
    ~Picture();
    /**
     * @brief
     *
     * @return int
     */
    int getHeight() const;
    /**
     * @brief
     *
     * @param value
     */
    void setHeight(int value);
    /**
     * @brief
     *
     * @return int
     */
    int getWidth() const;
    /**
     * @brief
     *
     * @param value
     */
    void setWidth(int value);
    /**
     * @brief
     *
     * @return unsigned char
     */
    unsigned char *getPixel() const;
    /**
     * @brief
     *
     * @param value
     */
    void setPixel(unsigned char *value);
    /**
     * @brief
     *
     * @param p_toCopy
     * @return Picture &operator
     */
    Picture& operator=(const Picture& p_toCopy);
};

#endif // PICTURE_H
