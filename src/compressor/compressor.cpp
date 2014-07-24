#include "compressor.h"


double Compressor::getCompressionRate() const
{
    return m_compressionRate;
}

long long Compressor::getTimeAgo() const
{
    return m_timeAgo;
}

int Compressor::getSizeOfRawData() const
{
    return m_sizeOfRawData;
}

int Compressor::getSizeOfCompData() const
{
    return m_sizeOfCompData;
}

int Compressor::getSizeOfCodeTable() const
{
    return m_sizeOfCodeTable;
}

unsigned char *Compressor::getRawData() const
{
    return m_rawData;
}

unsigned char *Compressor::getCompData() const
{
    return m_compData;
}

unsigned char *Compressor::getCodeTable() const
{
    return m_codeTable;
}

Compressor::Compressor()
{
}
