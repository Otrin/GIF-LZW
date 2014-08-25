#ifndef GIF_H
#define GIF_H
#include "frame.h"
#include "picture.h"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

/**
 * @brief
 *
 */
class Gif : public Picture
{
    int m_gctFlag, /**< global-color-table-flag */
    m_sizeOfGCT, /**< size of the gct */
    m_bgColor, /**<  */
    m_sizeOfFrames, /**< number of frames in the gif */
    m_ownGCT; /**< flag, if the GCT is own created */
    vector<Frame*> m_frames; /**< frames in the gif */
    unsigned char* m_GCT; /**<  */
    char m_bgRed, /**<  */
    m_bgGreen, /**<  */
    m_bgBlue; /**<  */
public:
    /**
     * @brief
     *
     */
    Gif();
	Gif(const Gif& p_toCopy);
    /**
     * @brief
     *
     */
    ~Gif();
    /**
     * @brief retruns a frame
     *
     * @param frame index of the frame
     * @return Frame the frame
     */
    Frame *getFrame(int frame);
    /**
     * @brief
     *
     * @return unsigned char
     */
    unsigned char *getGCT() const;
    /**
     * @brief
     *
     * @param p_value
     * @param p_size
     */
    void setGCT(unsigned char *p_value, int p_size);
    /**
     * @brief
     *
     * @param p_value
     * @param p_size
     */
    void setGCT(vector<unsigned char> p_value, int p_size);
    /**
     * @brief
     *
     * @return int
     */
    int getGctFlag() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setGctFlag(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getSizeOfGCT() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setSizeOfGCT(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getBgColor() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setBgColor(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getSizeOfFrames() const;
    /**
     * @brief extends the gif to one more frame
     *
     */
    void extendFrames();
    /**
     * @brief
     *
     * @param p_toCopy
     * @return Gif &operator
     */
    Gif& operator=(const Gif& p_toCopy);
};

#endif // GIF_H
