#ifndef RUNLENGTHENCODING_H
#define RUNLENGTHENCODING_H
#include "compressor.h"

class RunLengthEncoding: public Compressor{
public:
    /**
     * @brief Defaultconstructor
     */
    RunLengthEncoding();
    /**
     * @brief Destructor
     */
    ~RunLengthEncoding();
    /**
     * @param p_rawData - rawdata to compress
     * @param p_sizeOfRawData - size of rawdata in bytes
     * @param p_sizeCodeTable - unused here
     * @return
     */
	unsigned char* encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable);
    /**
     * @param p_compData - compressed data to uncompress
     * @param p_sizeOfCompData - size of compressed data in bytes
     * @param p_codeTable - unused here
     * @param p_sizeOfCodeTable - unused here
     * @return
     */
	unsigned char* decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable, int p_sizeOfOutput);
};

#endif // RUNLENGTHENCODING_H
