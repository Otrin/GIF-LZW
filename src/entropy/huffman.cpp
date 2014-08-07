#include "huffman.h"
#include "node.h"
#include <stdio.h>
#include <iostream>
#include <map>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;

void inorder(Node* node);

Huffman::Huffman(){
    m_maxSizeOfNodeArray = 0;
    m_currentSizeOfNodeArray = 0;
    m_nodes = NULL;
}

Huffman::~Huffman(){

}

unsigned char* Huffman::encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable){
    if(p_sizeOfRawData == 0 || p_rawData == NULL){
        throw "No Data!";
    }
    m_sizeOfRawData = p_sizeOfRawData;

    auto startTime = system_clock::now();
    map<unsigned char, int> analyzeMap;

    //analyze rawdata
    for (int i = 0; i < p_sizeOfRawData; ++i) {
        analyzeMap[p_rawData[i]]++;
    }

    m_maxSizeOfNodeArray = analyzeMap.size()+1;
//    cout << m_maxSizeOfNodeArray << endl;
    m_nodes = new Node[m_maxSizeOfNodeArray];
    m_nodes[m_currentSizeOfNodeArray] = Node('\0', -INT_MAX);

    //set up nodearray
    for(map<unsigned char, int>::iterator iter = analyzeMap.begin() ;iter != analyzeMap.end(); iter++){
        Node *temp = new Node(iter->first, iter->second);
        insert(temp);
    }

    //build tree
     for (int i = 0; i < analyzeMap.size()-1 ; ++i) {
        Node *left = &deleteMin();
        Node *right = &deleteMin();
        Node *temp = new Node('\0', left->getFrequenzy() + right->getFrequenzy());
        temp->setLeft(left);
        temp->setRight(right);
        insert(temp);
    }


    Node *tree = &deleteMin();
    setUpCodeMap(tree, "");

    //calculate size of compdata
    for (int i = 0; i < p_sizeOfRawData; ++i) {
        m_sizeOfCompData+= m_codeMap[p_rawData[i]].size();
    }

    //compress rawdata
    int bytes = m_sizeOfCompData%8==0?m_sizeOfCompData/8:m_sizeOfCompData/8+1;
    m_compData = new unsigned char[m_sizeOfCompData];
    int sizeOfCode, overflow = 0, byteCounter = 0;
    string currentCode = "";
    for (int i = 0; i < p_sizeOfRawData; ++i) {
        currentCode = m_codeMap[p_rawData[i]];
        sizeOfCode = currentCode.size();
        for (int j = 0; j < sizeOfCode; ++j) {
            if(overflow == 8){
                byteCounter++;
                overflow = 0;
            }
            m_compData[byteCounter] <<= 1;
            if(currentCode[j] == 'I'){
                m_compData[byteCounter] += 1;
            }
            overflow++;
        }
    }

    //shifting last bits to right position
    for (int i = 0; i < bytes*8-m_sizeOfCompData; ++i) {
        m_compData[bytes-1] <<= 1;
    }

//    for (int i = 0; i < m_sizeOfCompData/8+1; ++i) {
//        for (int j = 1<<7; j > 0; j/=2) {
//            cout << (m_compData[i]&j?"I":"0");
//        }
//    }

    m_compressionRate = (double)p_sizeOfRawData*8/m_sizeOfCompData;

    //calculate size of codetable
    m_sizeOfCodeTable = m_codeMap.size()*2;
    for(map<unsigned char, string>::iterator iter = m_codeMap.begin(); iter != m_codeMap.end(); iter++){
        m_sizeOfCodeTable += iter->second.size()%8==0?iter->second.size()/8:iter->second.size()/8+1;
    }

    //create codetable
    m_codeTable = new unsigned char[m_sizeOfCodeTable];
    int counter = 0;
    for(map<unsigned char, string>::iterator iter = m_codeMap.begin(); iter != m_codeMap.end(); iter++){
        m_codeTable[counter] = iter->first;
        counter++;
        currentCode = iter->second;
        m_codeTable[counter] = currentCode.size();
        counter++;
        bytes = currentCode.size()%8==0?currentCode.size()/8:currentCode.size()/8+1;
        overflow = 0;
        for (int i = 0; i < currentCode.size(); ++i) {
            if(overflow == 8){
                overflow = 0;
                counter++;
            }
            m_codeTable[counter] <<= 1;
            if(currentCode[i]=='I'){
                m_codeTable[counter] += 1;
            }
            overflow++;
        }
        for (int i = 0; i < bytes*8-currentCode.size(); ++i) {
            m_codeTable[counter] <<= 1;
        }
        counter++;
    }

    //print codeTable

//    for(map<unsigned char, string>::iterator iter = m_codeMap.begin(); iter != m_codeMap.end(); iter++){
//        cout << (int)iter->first << " => " << iter->second << endl;
//    }

//    for (int i = 0; i < m_sizeOfCodeTable; i+=3) {
//        cout << "value: " << (int)m_codeTable[i] << " size: " << (int)m_codeTable[i+1] << " code: ";
//        int c = 0, v = 2;
//        int j = 1<<7;
//        while(c!=m_codeTable[i+1]){
//            if(j == 0){
//                j = 1<<7;
//                v++;
//            }
//            m_codeTable[i+v]&j?printf("I"):printf("0");
//            c++;
//            j /= 2;
//        }
//        i+=v-2;
//        cout << endl;
//    }

    nanoseconds time = system_clock::now()-startTime;
    m_timeAgo = time.count();

    return  m_compData;
}

unsigned char* Huffman::decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable, int p_sizeOfOutput){
    if(p_sizeOfCompData == 0 || p_compData== NULL){
        throw "no Data!";
    }
    m_sizeOfCompData = p_sizeOfCompData;
    m_sizeOfCodeTable = p_sizeOfCodeTable;
    auto startTime = system_clock::now();
    map<string, char> codeMap;
    string currentCode;
    vector<unsigned char> uncompressedData;
    int sizeCounter, sizeOfCode;

    //set up codeMap
    for (int i = 0; i < m_sizeOfCodeTable; i+=3) {
        sizeCounter = 0;
        currentCode = "";
        sizeOfCode = p_codeTable[i+1];
        int j = 1<<7, k = 2;
        while(sizeCounter != sizeOfCode){
            if(j == 0){
                j = 1<<7;
                k++;
            }
            currentCode += m_codeTable[i+k]&j?"I":"0";
            sizeCounter++;
            j/=2;
        }
        codeMap[currentCode] = m_codeTable[i];
        i += k-2;
    }

    //print codetable
//    for(map<string, char>::iterator iter = codeMap.begin(); iter != codeMap.end(); iter++){
//        cout << (int)iter->second << " => " << iter->first << endl;
//    }

    //uncompress data
    currentCode = "";
    map<string, char>::iterator iter;
    int bytes = p_sizeOfCompData%8==0?p_sizeOfCompData/8:p_sizeOfCompData/8+1;
    for (int i = 0; i < bytes; ++i) {
        for (int j = 1<<7; j > 0; j/=2) {
            currentCode += p_compData[i]&j?"I":"0";
            iter = codeMap.find(currentCode);
            if(iter != codeMap.end()){
                uncompressedData.push_back(codeMap[currentCode]);
                currentCode = "";
                //                break;
            }
        }
    }

    m_sizeOfRawData = uncompressedData.size();
    m_rawData = new unsigned char[m_sizeOfRawData];
    for (int i = 0; i < m_sizeOfRawData; ++i) {
        m_rawData[i] = uncompressedData[i];
    }

    nanoseconds time = system_clock::now()-startTime;
    m_timeAgo = time.count();

    return m_rawData;
}

void Huffman::insert(Node *node){
    m_currentSizeOfNodeArray++;
    m_nodes[m_currentSizeOfNodeArray] = *node;
    int insert = m_currentSizeOfNodeArray;
    while(m_nodes[insert/2].getFrequenzy() > node->getFrequenzy()){
        m_nodes[insert] = m_nodes[insert/2];
        insert /= 2;
    }
    m_nodes[insert] = *node;
}

Node& Huffman::deleteMin(){
    Node *minElement, *lastElement;
    int child, now;
    minElement = new Node(m_nodes[1]);
    lastElement = new Node(m_nodes[m_currentSizeOfNodeArray]);
    m_currentSizeOfNodeArray--;
    for(now = 1; now*2 <= m_currentSizeOfNodeArray; now = child){
        child = now*2;
        if(child != m_currentSizeOfNodeArray && m_nodes[child+1].getFrequenzy() < m_nodes[child].getFrequenzy()){
            child++;
        }
        if(lastElement->getFrequenzy() > m_nodes[child].getFrequenzy()){
            m_nodes[now] = m_nodes[child];
        }else{
            break;
        }
    }
    m_nodes[now] = *lastElement;
    return *minElement;
}

void Huffman::setUpCodeMap(Node *temp, string code){
    if(&temp->getLeft() == NULL && &temp->getRight() == NULL){
         m_codeMap[temp->getValue()] = code;
//         cout << temp->getValue() << " => " << code << endl;
    }
    string leftCode = "", rightCode = "";
    leftCode += code;
    rightCode += code;
    leftCode += "0";
    rightCode += "I";
    if(&temp->getLeft() != NULL){
        setUpCodeMap(&temp->getLeft(), leftCode);
    }
    if(&temp->getRight() != NULL){
        setUpCodeMap(&temp->getRight(), rightCode);
    }
}


void inorder(Node* node){
    cout << node->getFrequenzy();
    if(&node->getLeft() != NULL){
        cout << "left\n";
        inorder(&node->getLeft());
    }
    if(&node->getRight() != NULL){
        cout << "right\n";
        inorder(&node->getRight());
    }
}
