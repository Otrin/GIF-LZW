#ifndef RUNLENGTHENCODING_H
#define RUNLENGTHENCODING_H
#include <string>

class RunLengthEncoding{
private:
    char *m_rawData, *m_compressedData;
    int m_sizeOfRawData, m_sizeOfCompressedData;
    double m_compressionRate;
    long m_compressionTime;
    bool m_encode;
public:
    RunLengthEncoding();
    char* encode(int sizeOfRawData, char *rawData);
    char* decode(int sizeOfCompressedData, char *compressedData, int sizeOffCodeTable, char *codeTable);
    int getSizeOfCompressedData();
    int getSizeOfRawData();
    std::string getStatistics();
    void printStatistics();
};

#endif // RUNLENGTHENCODING_H
