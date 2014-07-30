#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "compressor.h"
#include "node.h"
#include <string>
#include <map>

class Huffman:public Compressor{
private:
    //Node array needed to create codeTable
    Node* m_nodes;
    //codemap
    std::map<unsigned char, std::string> m_codeMap;
    //some helper variables
    int m_maxSizeOfNodeArray, m_currentSizeOfNodeArray;
    /**
     * @brief rteturns the smallest node from nodeArray
     */
    Node& deleteMin();
    /**
     * @brief inserts the node into the nodeArray
     */
    void insert(Node *node);
    /**
     * @brief helper function to set up huffmantree
     */
    void setUpCodeMap(Node *temp, std::string code);
public:
    /**
     * @brief default constructor
     */
    Huffman();
    /**
     * @brief destructor
     */
    ~Huffman();
    /**
     * @param p_rawData - rawdata
     * @param p_sizeOfRawData - size of rawdata in BYTES
     * @param p_sizeCodeTable - not needed here
     * @return compresseddata
     */
    unsigned char* encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable);
    /**
     * @param p_compData - compressed data
     * @param p_sizeOfCompData - size of compressed data in BITS
     * @param p_codeTable - codetable
     * @param p_sizeOfCodeTable - size of code tayble in BYTES
     * @return rawdata
     */
    unsigned char* decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable);
};

#endif // HUFFMAN_H
