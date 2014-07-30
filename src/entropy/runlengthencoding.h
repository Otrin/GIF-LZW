#ifndef RUNLENGTHENCODING_H
#define RUNLENGTHENCODING_H
#include "compressor.h"

class RunLengthEncoding: public Compressor{
public:
    RunLengthEncoding();
    ~RunLengthEncoding();
    unsigned char* encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable);
    unsigned char* decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable);
};

#endif // RUNLENGTHENCODING_H
