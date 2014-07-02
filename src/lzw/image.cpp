#include "image.h"


int Image::getTop() const
{
    return m_top;
}

void Image::setTop(int p_value)
{
    m_top = p_value;
}

int Image::getLeft() const
{
    return m_left;
}

void Image::setLeft(int p_value)
{
    m_left = p_value;
}

int Image::getHeight() const
{
    return m_height;
}

void Image::setHeight(int p_value)
{
    m_height = p_value;
}

int Image::getDelayTime() const
{
    return m_delayTime;
}

void Image::setDelayTime(int p_value)
{
    m_delayTime = p_value;
}

int Image::getTranspColorIndex() const
{
    return m_transpColorIndex;
}

void Image::setTranspColorIndex(int p_value)
{
    m_transpColorIndex = p_value;
}

int Image::getTranspColorFlag() const
{
    return m_transpColorFlag;
}

void Image::setTranspColorFlag(int p_value)
{
    m_transpColorFlag = p_value;
}

int Image::getLctFlag() const
{
    return m_lctFlag;
}

void Image::setLctFlag(int p_value)
{
    m_lctFlag = p_value;
}

int Image::getSortFlag() const
{
    return m_sortFlag;
}

void Image::setSortFlag(int p_value)
{
    m_sortFlag = p_value;
}

int Image::getSizeOfLCT() const
{
    return m_sizeOfLCT;
}

void Image::setSizeOfLCT(int p_value)
{
    m_sizeOfLCT = p_value;
}

char *Image::getLct() const
{
    return m_LCT;
}

void Image::setLct(char *p_value, int p_size)
{
    m_sizeOfLCT = p_size;
    delete[] m_LCT;
    m_LCT = new char[m_sizeOfLCT];
    for(int i = 0; i<m_sizeOfLCT; ++i){
        m_LCT[i] = p_value[i];
    }
}

void Image::setLct(std::vector<char> p_colors, int p_n)
{
    m_sizeOfLCT = p_n;
    delete[] m_LCT;
    m_LCT = new char[p_n];
    std::copy(p_colors.begin(), p_colors.end(), m_LCT);
}

unsigned char *Image::getCodeTable()
{
    return m_codeTable;
}

void Image::setCodeTable(unsigned char *p_value, int p_size)
{
    m_sizeOfCodeTable = p_size;
    delete[] m_codeTable;
    m_codeTable = new unsigned char[m_sizeOfCodeTable];
    for(int i = 0; i<m_sizeOfCodeTable; ++i){
        m_codeTable[i] = p_value[i];
    }
}

int Image::getWidth() const
{
    return m_width;
}

void Image::setWidth(int p_value)
{
    m_width = p_value;
}

int Image::getMinCodeSize() const
{
    return m_minCodeSize;
}

void Image::setMinCodeSize(int p_value)
{
    m_minCodeSize = p_value;
}

int Image::getSizeOfCodeTable() const
{
    return m_sizeOfCodeTable;
}

void Image::setSizeOfCodeTable(int p_value)
{
    unsigned char *old = m_codeTable;
    m_codeTable = new unsigned char[p_value];
    for(int i = 0; i<m_sizeOfCodeTable; ++i){
        m_codeTable[i] = old[i];
    }
    delete[] old;
    m_sizeOfCodeTable = p_value;
}

char *Image::getPixel() const
{
    return m_pixel;
}

void Image::setPixel(char *p_value, int p_size)
{
    m_sizeOfPixel = p_size;
    delete[] m_pixel;
    m_pixel = new char[m_sizeOfPixel];
    for(int i = 0; i<m_sizeOfPixel; ++i){
        m_pixel[i] = p_value[i];
    }
}


int Image::getSizeOfPixel() const
{
    return m_sizeOfPixel;
}

void Image::setSizeOfPixel(int p_value)
{
    m_sizeOfPixel = p_value;
}

Image &Image::operator=(const Image &p_toCopy)
{
    if(&p_toCopy != this){
        m_left = p_toCopy.m_left;
        m_top = p_toCopy.m_top;
        m_width = p_toCopy.m_width;
        m_height = p_toCopy.m_height;
        m_delayTime = p_toCopy.m_delayTime;
        m_transpColorIndex = p_toCopy.m_transpColorIndex;
        m_transpColorFlag = p_toCopy.m_transpColorFlag;
        m_lctFlag = p_toCopy.m_lctFlag;
        m_sortFlag = p_toCopy.m_sortFlag;
        m_sizeOfLCT = p_toCopy.m_sizeOfLCT;
        m_sizeOfCodeTable = p_toCopy.m_sizeOfCodeTable;
        m_sizeOfPixel = p_toCopy.m_sizeOfPixel;
        m_minCodeSize = p_toCopy.m_minCodeSize;

        m_LCT = new char[m_sizeOfLCT];
        m_codeTable = new unsigned char[m_sizeOfCodeTable];
        m_pixel = new char[m_sizeOfPixel];

        for (int i = 0; i < m_sizeOfLCT; ++i) {
            m_LCT[i] = p_toCopy.m_LCT[i];
        }


        for (int i = 0; i < m_sizeOfCodeTable; ++i) {
            m_codeTable[i] = p_toCopy.m_codeTable[i];
        }

        for (int i = 0; i < m_sizeOfPixel; ++i) {
            m_pixel[i] = p_toCopy.m_pixel[i];
        }
    }
}



int Image::getInterlaceFlag() const
{
    return m_interlaceFlag;
}

void Image::setInterlaceFlag(int p_value)
{
    m_interlaceFlag = p_value;
}

int Image::getDisposualMethod() const
{
    return m_disposualMethod;
}

void Image::setDisposualMethod(int p_disposualMethod)
{
    m_disposualMethod = p_disposualMethod;
}

int Image::getUserInputFlag() const
{
    return m_UserInputFlag;
}

void Image::setUserInputFlag(int p_UserInputFlag)
{
    m_UserInputFlag = p_UserInputFlag;
}
Image::Image()
{
    m_codeTable = new unsigned char[1];
    m_sizeOfCodeTable = 1;
    m_LCT = new char[1];
    m_sizeOfLCT = 1;
    m_pixel = new char[1];
    m_sizeOfPixel = 1;
}

Image::~Image()
{
    delete[] m_pixel;
    delete[] m_LCT;
    delete[] m_codeTable;
}
