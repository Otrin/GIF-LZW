#ifndef HUFFMAN2_H
#define HUFFMAN2_H

#include "compressor.h"
#include "node.h"
#include <string>
#include <map>

class Huffman2:public Compressor{
private:
    Node* m_nodes;
    std::map<unsigned char, std::string> m_codeMap;
    Node& deleteMin();
    int m_maxSizeOfNodeArray, m_currentSizeOfNodeArray;
    void insert(Node *node);
    void setUpCodeMap(Node *temp, std::string code);
public:
    Huffman2();
    ~Huffman2();
    unsigned char* encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable);
    unsigned char* decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable);
};

#endif // HUFFMAN2_H
