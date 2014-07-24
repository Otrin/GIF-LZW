#ifndef COMPRESSOR_H
#define COMPRESSOR_H

class Compressor
{
protected:
    long long m_timeAgo;
    double m_compressionRate;
    int m_sizeOfRawData, m_sizeOfCompData, m_sizeOfCodeTable;
    unsigned char *m_rawData, *m_compData, *m_codeTable;
public:
    Compressor();
    virtual unsigned char* encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable) = 0;
    virtual unsigned char* decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable) = 0;
    double getCompressionRate() const;
    long long getTimeAgo() const;
    int getSizeOfRawData() const;
    int getSizeOfCompData() const;
    int getSizeOfCodeTable() const;
    unsigned char *getRawData() const;
    unsigned char *getCompData() const;
    unsigned char *getCodeTable() const;
};

#endif // COMPRESSOR_H
