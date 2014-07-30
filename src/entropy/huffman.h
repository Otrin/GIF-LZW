#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <string>
#include "compressor.h"

class Huffman: public Compressor{
public:
    Huffman();
    ~Huffman();
    unsigned char* encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable);
    unsigned char* decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable);
};

#endif // HUFFMAN_H
