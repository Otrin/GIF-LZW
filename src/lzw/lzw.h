#ifndef LZW_H
#define LZW_H

class LZW
{
char* alphabet;
long long timeAgo = 0; //i can still remember;)
unsigned int getBits(const unsigned char* rawData, int pos, int currentCodeSize);
public:
    LZW();
    char* encode(char* compData);
    char* getAlphabet();
    char *decode(unsigned char *rawData, int sizeRawData, char *alphabet, int sizeAlphabet, int mode);
};

#endif // LZW_H
