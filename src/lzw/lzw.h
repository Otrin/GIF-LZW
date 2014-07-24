#ifndef LZW_H
#define LZW_H

#include "codeword.h"
#include  "gif.h"
#include "compressor.h"
#include <vector>

class LZW: public Compressor
{
static std::vector<CodeWord> table;
static unsigned int getBits(const unsigned char* rawData, int pos, int currentCodeSize);
static void inCompData(int code, std::vector<unsigned char> &compData, int currentCodeLength, int currentBit);
static std::vector<int> tableContains(std::vector<CodeWord>table, CodeWord buffer, int k = -1);
static std::vector<int> tableContains(std::vector<CodeWord>table, std::vector<int> lastPosInTable, CodeWord indexBuffer, int k = -1);
static void resetLastPosInTable(std::vector<int> &table, int n);
public:
    static  long long timeAgo; //i can still remember;)
    LZW();
    unsigned char* encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable);
    unsigned char* decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable);
    unsigned char* decode(Gif p_gif, int p_frame);
    unsigned char* encode(Gif p_gif, int p_frame);
    static unsigned int nextStep(unsigned int lastCode, int currentCodeSize, unsigned int tableLength, unsigned char* rawData, int currentBit, int startCodeSize);

};

#endif // LZW_H
