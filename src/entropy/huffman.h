#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "compressor.h"
#include "node.h"
#include <string>
#include <map>

class Huffman:public Compressor{
private:
    Node* m_nodes;
    std::map<unsigned char, std::string> m_codeMap;
    Node& deleteMin();
    int m_maxSizeOfNodeArray, m_currentSizeOfNodeArray;
    void insert(Node *node);
    void setUpCodeMap(Node *temp, std::string code);
public:
    Huffman();
    ~Huffman();
    unsigned char* encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable);
    unsigned char* decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable);
};

#endif // HUFFMAN_H
