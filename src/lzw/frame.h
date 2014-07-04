#ifndef IMAGE_H
#define IMAGE_H
#include <vector>

class Frame
{
int m_left, m_top, m_width, m_height, m_delayTime, m_transpColorIndex,
m_transpColorFlag, m_lctFlag, m_sortFlag, m_sizeOfLCT, m_sizeOfCodeTable,
m_sizeOfPixel, m_minCodeSize, m_interlaceFlag, m_disposalMethod,
m_UserInputFlag;
char* m_LCT;
unsigned char* m_codeTable;
char* m_pixel;
public:
    Frame();
    ~Frame();
    int getTop() const;
    void setTop(int p_value);
    int getLeft() const;
    void setLeft(int p_value);
    int getHeight() const;
    void setHeight(int p_value);
    int getDelayTime() const;
    void setDelayTime(int p_value);
    int getTranspColorIndex() const;
    void setTranspColorIndex(int p_value);
    int getTranspColorFlag() const;
    void setTranspColorFlag(int p_value);
    int getLctFlag() const;
    void setLctFlag(int p_value);
    int getSortFlag() const;
    void setSortFlag(int p_value);
    int getSizeOfLCT() const;
    void setSizeOfLCT(int p_value);
    char *getLct() const;
    void setLct(char *p_value, int p_size);
    void setLct(std::vector<char> p_colors, int p_n);
    unsigned char *getCodeTable();
    void setCodeTable(unsigned char *p_value, int p_size);
    int getWidth() const;
    void setWidth(int p_value);
    int getMinCodeSize() const;
    void setMinCodeSize(int p_value);
    int getSizeOfCodeTable() const;
    void setSizeOfCodeTable(int p_value);
    char *getPixel() const;
    void setPixel(char *p_value, int p_size);
    int getSizeOfPixel() const;
    void setSizeOfPixel(int p_value);
    Frame& operator=(const Frame& p_toCopy);
    int getInterlaceFlag() const;
    void setInterlaceFlag(int p_value);
    int getDisposualMethod() const;
    void setDisposualMethod(int p_disposualMethod);
    int getUserInputFlag() const;
    void setUserInputFlag(int p_UserInputFlag);
};

#endif // IMAGE_H
