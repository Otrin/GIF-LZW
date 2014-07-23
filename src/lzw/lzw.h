#ifndef LZW_H
#define LZW_H

#include "codeword.h"
#include  "gif.h"
#include <vector>

class LZW
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
    static unsigned char* encode(char *p_uncompData, int p_size, char* p_alphabet, int p_sizeAlphabet, int &p_codeSize);
    static char *decode(unsigned char *rawData, int sizeRawData, char *alphabet, int sizeAlphabet, int mode, int countPixel);
    static unsigned int nextStep(unsigned int lastCode, int currentCodeSize, unsigned int tableLength, unsigned char* rawData, int currentBit, int startCodeSize);

};

#endif // LZW_H
