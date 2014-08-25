#ifndef IMAGE_H
#define IMAGE_H
#include <vector>

/**
 * @brief
 *
 */
class Frame
{
int m_left,  /**<  */
m_top,  /**<  */
m_width,  /**<  */
m_height,  /**<  */
m_delayTime,  /**<  */
m_transpColorIndex, /**<  */
m_transpColorFlag,  /**<  */
m_lctFlag,  /**< local-color-table-flag */
m_sortFlag,  /**<  */
m_sizeOfLCT,  /**<  */
m_sizeOfData, /**< size of the raw or compressed data */
m_sizeOfPixel,  /**<  */
m_minCodeSize,  /**< minimum-code-size al the begin of the lzw */
m_interlaceFlag,  /**< */
m_disposalMethod, /**<  */
m_UserInputFlag; /**<  */
bool m_dataFlag; /**< this shows, if the m_data is compData oder rawData. 1 = compData, 0 = rawData; */
unsigned char* m_LCT; /**<  */
unsigned char* m_data; /**<  */
unsigned char* m_pixel; /**<  */
public:
    /**
     * @brief
     *
     */
    Frame();
    /**
     * @brief
     *
     * @param p_toCopy
     */
    Frame(const Frame& p_toCopy);
    /**
     * @brief
     *
     */
    ~Frame();
    /**
     * @brief
     *
     * @return int
     */
    int getTop() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setTop(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getLeft() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setLeft(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getHeight() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setHeight(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getDelayTime() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setDelayTime(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getTranspColorIndex() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setTranspColorIndex(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getTranspColorFlag() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setTranspColorFlag(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getLctFlag() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setLctFlag(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getSortFlag() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setSortFlag(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getSizeOfLCT() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setSizeOfLCT(int p_value);
    /**
     * @brief
     *
     * @return unsigned char
     */
    unsigned char *getLct() const;
    /**
     * @brief fills the lct with an array
     *
     * @param p_value
     * @param p_size
     */
    void setLct(unsigned char *p_value, int p_size);
    /**
     * @brief fills the lct with an vector
     *
     * @param p_colors
     * @param p_n
     */
    void setLct(std::vector<unsigned char> p_colors, int p_n);
    /**
     * @brief
     *
     * @return unsigned char
     */
    unsigned char *getData();
    /**
     * @brief fills the data with an array
     *
     * @param p_value
     * @param p_size
     */
    void setData(unsigned char *p_value, int p_size);
    /**
     * @brief fills the data with an vector
     *
     * @param p_value
     */
    void setData(std::vector<unsigned char> p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getWidth() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setWidth(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getMinCodeSize() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setMinCodeSize(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getSizeOfData() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setSizeOfCodeTable(int p_value);
    /**
     * @brief
     *
     * @return unsigned char
     */
    unsigned char *getPixel() const;
    /**
     * @brief
     *
     * @param p_value
     * @param p_size
     */
    void setPixel(unsigned char *p_value, int p_size);
    /**
     * @brief
     *
     * @return int
     */
    int getSizeOfPixel() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setSizeOfPixel(int p_value);
    /**
     * @brief
     *
     * @param p_toCopy
     * @return Frame &operator
     */
    Frame& operator=(const Frame& p_toCopy);
    /**
     * @brief
     *
     * @return int
     */
    int getInterlaceFlag() const;
    /**
     * @brief
     *
     * @param p_value
     */
    void setInterlaceFlag(int p_value);
    /**
     * @brief
     *
     * @return int
     */
    int getDisposualMethod() const;
    /**
     * @brief
     *
     * @param p_disposualMethod
     */
    void setDisposualMethod(int p_disposualMethod);
    /**
     * @brief
     *
     * @return int
     */
    int getUserInputFlag() const;
    /**
     * @brief
     *
     * @param p_UserInputFlag
     */
    void setUserInputFlag(int p_UserInputFlag);
    /**
     * @brief
     *
     * @return bool
     */
    bool getDataFlag() const;
    /**
     * @brief
     *
     * @param getDataFlag
     */
    void setDataFlag(bool getDataFlag);
    /**
     * @brief creates a clone af this frame
     *
     * @return Frame a deep copy of this frame
     */
    Frame* clone();
};

#endif // IMAGE_H
