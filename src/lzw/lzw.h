#ifndef LZW_H
#define LZW_H

class LZW
{
static unsigned int getBits(const unsigned char* rawData, int pos, int currentCodeSize);
public:
    static  long long timeAgo; //i can still remember;)
    LZW();
    static char* encode(char* compData);
    static char *decode(unsigned char *rawData, int sizeRawData, char *alphabet, int sizeAlphabet, int mode, int countPixel);
    static unsigned int nextStep(unsigned int lastCode, int currentCodeSize, unsigned int tableLength, unsigned char* rawData, int currentBit, int startCodeSize);
};

#endif // LZW_H
