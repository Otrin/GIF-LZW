#include "frame.h"


int Frame::getTop() const
{
    return m_top;
}

void Frame::setTop(int p_value)
{
    m_top = p_value;
}

int Frame::getLeft() const
{
    return m_left;
}

void Frame::setLeft(int p_value)
{
    m_left = p_value;
}

int Frame::getHeight() const
{
    return m_height;
}

void Frame::setHeight(int p_value)
{
    m_height = p_value;
}

int Frame::getDelayTime() const
{
    return m_delayTime;
}

void Frame::setDelayTime(int p_value)
{
    m_delayTime = p_value;
}

int Frame::getTranspColorIndex() const
{
    return m_transpColorIndex;
}

void Frame::setTranspColorIndex(int p_value)
{
    m_transpColorIndex = p_value;
}

int Frame::getTranspColorFlag() const
{
    return m_transpColorFlag;
}

void Frame::setTranspColorFlag(int p_value)
{
    m_transpColorFlag = p_value;
}

int Frame::getLctFlag() const
{
    return m_lctFlag;
}

void Frame::setLctFlag(int p_value)
{
    m_lctFlag = p_value;
}

int Frame::getSortFlag() const
{
    return m_sortFlag;
}

void Frame::setSortFlag(int p_value)
{
    m_sortFlag = p_value;
}

int Frame::getSizeOfLCT() const
{
    return m_sizeOfLCT;
}

void Frame::setSizeOfLCT(int p_value)
{
    m_sizeOfLCT = p_value;
}

char *Frame::getLct() const
{
    return m_LCT;
}

void Frame::setLct(char *p_value, int p_size)
{
    m_sizeOfLCT = p_size;
    delete[] m_LCT;
    m_LCT = new char[m_sizeOfLCT];
    for(int i = 0; i<m_sizeOfLCT; ++i){
        m_LCT[i] = p_value[i];
    }
}

void Frame::setLct(std::vector<char> p_colors, int p_n)
{
    m_sizeOfLCT = p_n;
    delete[] m_LCT;
    m_LCT = new char[p_n];
    std::copy(p_colors.begin(), p_colors.end(), m_LCT);
}

unsigned char *Frame::getCodeTable()
{
    return m_codeTable;
}

void Frame::setCodeTable(unsigned char *p_value, int p_size)
{
    m_sizeOfCodeTable = p_size;
    delete[] m_codeTable;
    m_codeTable = new unsigned char[m_sizeOfCodeTable];
    for(int i = 0; i<m_sizeOfCodeTable; ++i){
        m_codeTable[i] = p_value[i];
    }
}

int Frame::getWidth() const
{
    return m_width;
}

void Frame::setWidth(int p_value)
{
    m_width = p_value;
}

int Frame::getMinCodeSize() const
{
    return m_minCodeSize;
}

void Frame::setMinCodeSize(int p_value)
{
    m_minCodeSize = p_value;
}

int Frame::getSizeOfCodeTable() const
{
    return m_sizeOfCodeTable;
}

void Frame::setSizeOfCodeTable(int p_value)
{
    unsigned char *old = m_codeTable;
    m_codeTable = new unsigned char[p_value];
    for(int i = 0; i<m_sizeOfCodeTable; ++i){
        m_codeTable[i] = old[i];
    }
    delete[] old;
    m_sizeOfCodeTable = p_value;
}

char *Frame::getPixel() const
{
    return m_pixel;
}

void Frame::setPixel(char *p_value, int p_size)
{
    m_sizeOfPixel = p_size;
    delete[] m_pixel;
    m_pixel = new char[m_sizeOfPixel];
    for(int i = 0; i<m_sizeOfPixel; ++i){
        m_pixel[i] = p_value[i];
    }
}


int Frame::getSizeOfPixel() const
{
    return m_sizeOfPixel;
}

void Frame::setSizeOfPixel(int p_value)
{
    m_sizeOfPixel = p_value;
}

Frame &Frame::operator=(const Frame &p_toCopy)
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
    return *this;
}



int Frame::getInterlaceFlag() const
{
    return m_interlaceFlag;
}

void Frame::setInterlaceFlag(int p_value)
{
    m_interlaceFlag = p_value;
}

int Frame::getDisposualMethod() const
{
    return m_disposualMethod;
}

void Frame::setDisposualMethod(int p_disposualMethod)
{
    m_disposualMethod = p_disposualMethod;
}

int Frame::getUserInputFlag() const
{
    return m_UserInputFlag;
}

void Frame::setUserInputFlag(int p_UserInputFlag)
{
    m_UserInputFlag = p_UserInputFlag;
}
Frame::Frame()
{
    m_codeTable = new unsigned char[1];
    m_sizeOfCodeTable = 1;
    m_LCT = new char[1];
    m_sizeOfLCT = 1;
    m_pixel = new char[1];
    m_sizeOfPixel = 1;
}

Frame::~Frame()
{
    delete[] m_pixel;
    delete[] m_LCT;
    delete[] m_codeTable;
}
