#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <map>

class Huffman{
private:
    char *m_rawData, *m_compressedData, *m_codeTable;
    int m_sizeOfRawData, m_sizeOfCompressedData, m_sizeOfCodeTable;
    double m_compressionRate;
    long m_compressionTime;
    std::map<char, std::string> m_codeMap;
    bool m_encode;
public:
    Huffman();
    char* encode(int sizeOfRawData, char *rawData);
    char* decode(int sizeOfCompressedData, char *compressedData, int sizeOffCodeTable, char *codeTable);
    char* getCodeTable();
    int getSizeOfCodeTable();
    int getSizeOfCompressedData();
    int getSizeOfRawData();
    std::string getStatistics();
    void printStatistics();
};

#endif // HUFFMAN_H
