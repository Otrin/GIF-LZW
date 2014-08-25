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
 * @brief returns a bitstream from the uncompressed data from the gif.
 *
 * @param rawData raw-data to uncompress
 * @param pos position of the bit, whiche will be retuned
 * @param currentCodeSize count, how many bits will be returned
 * @return unsigned int the int-value of the next "raw-code"
 */
static unsigned int getBits(const unsigned char* rawData, int pos, int currentCodeSize);
/**
 * @brief writes a code into the compressed-data
 *
 * @param code output to write in the compressed-data
 * @param compData the compressed data
 * @param currentCodeLength number of bits to encode the output
 * @param currentBit position in the compressed-data-vector
 */
static void inCompData(int code, std::vector<unsigned char> &compData, int currentCodeLength, int currentBit);

int m_currentCodeLength, // number of bits of the current output
m_startCodeLength, // saves the codelength from the beginning
m_currentBit, // position in the stream
m_posInTable, //current position in the table
m_lastPosInTable,  // last code in the encode-algorithm
m_sizeOfTable, //tablesize in the lzw-algorithm
m_sizeOfTableBackup, //backup of the tablesize
m_clearCode, //the value of the clearcode in the lzw-algorithm
m_endCode, //the value of the endCode in the lzw-algorithm
m_k, //the k-variable from the encode
m_i; // counter of the pixel at encode
std::vector<unsigned char> m_currentCompData; // compressed data in this step
std::vector<CodeWord> m_table, // lzw-table in the decode and encode
m_tableBackup; //bakup of the table -> wenn clearcode, the reinitialisize the table
int *m_highlightingArray; //array for the highlighting. pixel for pixel the positions in the table.
int m_pixelCounter, //number of pixels for the highlighting-array
m_highlightingGroup; //current number of pixels with the same index in the table
HashMap m_map, m_mapBackup; //hash-map for the lzw-table
CodeWord m_indexBuffer, //indexBuffer in teh lzw-algorithm
m_nextIndexBuffer; //next indexbuffer for the next step.

public:
    /**
     * @brief
     *
     */
    LZW();
    ~LZW();
    /**
     * @brief encode-function to encode uncompressed data
     *
     * @param p_rawData raw-data to compress
     * @param p_sizeOfRawData size of the data
     * @param p_sizeCodeTable size of the alphabet
     * @return unsigned char returns the compressed data
     */
	unsigned char* encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable);
    /**
     * @brief decode-function to decode lzw-compressed data
     *
     * @param p_compData compressed data
     * @param p_sizeOfCompData size of the data
     * @param p_codeTable the alphabet for the lzw-algorithm
     * @param p_sizeOfCodeTable size of the alphabet
     * @param p_sizeOfOutput size of the output->in gif = hight*width
     * @return unsigned char returns the uncompressed data
     */
    unsigned char* decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable, int p_sizeOfOutput);
    /**
     * @brief decodes a gif with the lzw-algorithm
     *
     * @param p_gif the gif to uncompress
     * @param p_frame the number of the frame in the gif
     * @return unsigned char returns the uncompressed data
     */
    unsigned char* decode(Gif& p_gif, int p_frame);
    /**
     * @brief encodes a gif with the lzw-algorithm
     *
     * @param p_gif the gif to compress
     * @param p_frame the number of the frame in the gif
     * @return unsigned char returns the compressed data
     */
    unsigned char* encode(Gif& p_gif, int p_frame);
    /**
     * @brief initialises the tables and the object to run the lzw-algorithm
     *
     * @param p_gif the gif to compress
     * @param p_frame the frame in the gif
     */
    void startEncode(Gif& p_gif, int p_frame);
    /**
     * @brief runs the lzw-encode for the next pixel
     *
     */
    void nextStep();
    /**
     * @brief ends the lzw-encode.
     *
     * @param p_gif the gif to compress
     * @param p_frame the frame in the gif
     */
    void endEncode(Gif& p_gif, int p_frame);
    /**
     * @brief returns the lzw-table
     *
     * @return std::vector<CodeWord> lzw-table
     */
    std::vector<CodeWord> getTable() const;
    /**
     * @brief reinitialisizes the lzw-object
     *
     */
    void resetInternalState();
    /**
     * @brief
     *
     * @return int returns the highlighting array
     */
    int *getHighlightingArray() const;
    /**
     * @brief ends the lzw-encode without a gif-object.
     *
     */
    void endEncode();
    int getI() const;
};

#endif // LZW_H
