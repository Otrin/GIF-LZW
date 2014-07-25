#ifndef LZW_H
#define LZW_H

#include "codeword.h"
#include  "gif.h"
#include "compressor.h"
#include "hashmap.h"
#include <vector>

class LZW: public Compressor
{
static unsigned int getBits(const unsigned char* rawData, int pos, int currentCodeSize);
static void inCompData(int code, std::vector<unsigned char> &compData, int currentCodeLength, int currentBit);
static std::vector<int> tableContains(std::vector<CodeWord>table, CodeWord buffer, int k = -1);
static std::vector<int> tableContains(std::vector<CodeWord>table, std::vector<int> lastPosInTable, CodeWord indexBuffer, int k = -1);
static void resetLastPosInTable(std::vector<int> &table, int n);
int m_currentCodeLength, m_startCodeLength, m_currentBit,
m_posInTable, m_lastPosInTable, m_sizeOfTable, m_sizeOfTableBackup,
m_clearCode, m_endCode, m_k, m_i;
std::vector<unsigned char> m_currentCompData;
std::vector<CodeWord> m_table, m_tableBackup;
HashMap m_map, m_mapBackup;
CodeWord m_indexBuffer, m_nextIndexBuffer;
public:
    LZW();
    unsigned char* encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable);
    unsigned char* decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable);
    unsigned char* decode(Gif p_gif, int p_frame);
    unsigned char* encode(Gif p_gif, int p_frame);
    void startEncode(Gif p_gif, int p_frame);
    void nextStep();
    void endEncode(Gif p_gif, int p_frame);
    std::vector<CodeWord> getTable() const;
};

#endif // LZW_H
