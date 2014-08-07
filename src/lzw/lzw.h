#ifndef LZW_H
#define LZW_H

#include "codeword.h"
#include  "gif.h"
#include "compressor.h"
#include "hashmap.h"
#include <vector>

/**
 * @brief
 *
 */
class LZW: public Compressor
{
/**
 * @brief
 *
 * @param rawData
 * @param pos
 * @param currentCodeSize
 * @return unsigned int
 */
static unsigned int getBits(const unsigned char* rawData, int pos, int currentCodeSize);
/**
 * @brief
 *
 * @param code
 * @param compData
 * @param currentCodeLength
 * @param currentBit
 */
static void inCompData(int code, std::vector<unsigned char> &compData, int currentCodeLength, int currentBit);
/**
 * @brief
 *
 * @param table
 * @param buffer
 * @param k
 * @return std::vector<int>
 */
static std::vector<int> tableContains(std::vector<CodeWord>table, CodeWord buffer, int k = -1);
/**
 * @brief
 *
 * @param table
 * @param lastPosInTable
 * @param indexBuffer
 * @param k
 * @return std::vector<int>
 */
static std::vector<int> tableContains(std::vector<CodeWord>table, std::vector<int> lastPosInTable, CodeWord indexBuffer, int k = -1);
/**
 * @brief
 *
 * @param table
 * @param n
 */
static void resetLastPosInTable(std::vector<int> &table, int n);
<<<<<<< HEAD
int m_currentCodeLength, m_startCodeLength, m_currentBit,
m_posInTable, m_lastPosInTable, m_sizeOfTable, m_sizeOfTableBackup,
m_clearCode, m_endCode, m_k, m_i;
std::vector<unsigned char> m_currentCompData;
std::vector<CodeWord> m_table, m_tableBackup;
int *m_highlightingArray;
HashMap m_map, m_mapBackup;
CodeWord m_indexBuffer, m_nextIndexBuffer;
=======
int m_currentCodeLength, m_startCodeLength, m_currentBit, /**< TODO */
m_posInTable, m_lastPosInTable, m_sizeOfTable, m_sizeOfTableBackup, /**< TODO */
m_clearCode, m_endCode, m_k, m_i; /**< TODO */
std::vector<unsigned char> m_currentCompData; /**< TODO */
std::vector<CodeWord> m_table, m_tableBackup; /**< TODO */
HashMap m_map, m_mapBackup; /**< TODO */
CodeWord m_indexBuffer, m_nextIndexBuffer; /**< TODO */
long long timeAgo; /**< TODO */
>>>>>>> eff0cc53d55dd7a68da11337ea4ee6beb58ff84b
public:
    /**
     * @brief
     *
     */
    LZW();
<<<<<<< HEAD
    ~LZW();
    unsigned char* encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable);
    unsigned char* decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable);
	unsigned char* decode(Gif& p_gif, int p_frame);
	unsigned char* encode(Gif& p_gif, int p_frame);
	void startEncode(Gif& p_gif, int p_frame);
=======
    /**
     * @brief
     *
     * @param p_rawData
     * @param p_sizeOfRawData
     * @param p_sizeCodeTable
     * @return unsigned char
     */
	unsigned char* encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable);
    /**
     * @brief
     *
     * @param p_compData
     * @param p_sizeOfCompData
     * @param p_codeTable
     * @param p_sizeOfCodeTable
     * @param p_sizeOfOutput
     * @return unsigned char
     */
    unsigned char* decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable, int p_sizeOfOutput);
    /**
     * @brief
     *
     * @param p_gif
     * @param p_frame
     * @return unsigned char
     */
    unsigned char* decode(Gif& p_gif, int p_frame);
    /**
     * @brief
     *
     * @param p_gif
     * @param p_frame
     * @return unsigned char
     */
    unsigned char* encode(Gif& p_gif, int p_frame);
    /**
     * @brief
     *
     * @param p_gif
     * @param p_frame
     */
    void startEncode(Gif& p_gif, int p_frame);
    /**
     * @brief
     *
     */
>>>>>>> eff0cc53d55dd7a68da11337ea4ee6beb58ff84b
    void nextStep();
    /**
     * @brief
     *
     * @param p_gif
     * @param p_frame
     */
    void endEncode(Gif& p_gif, int p_frame);
    /**
     * @brief
     *
     * @return std::vector<CodeWord>
     */
    std::vector<CodeWord> getTable() const;
    /**
     * @brief
     *
     */
    void resetInternalState();
};

#endif // LZW_H
