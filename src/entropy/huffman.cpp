#include "huffman.h"
#include "huffmannode.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <sstream>

using namespace std;
using namespace std::chrono;

//helper function to invert the codes
string invert(string code);

Huffman::Huffman(){
    m_sizeOfCodeTable = 0;
    m_sizeOfCompressedData = 0;
    m_sizeOfRawData = 0;
    m_compressionRate = 0;

}

char* Huffman::encode(int sizeOfRawData, char *rawData){
    if(sizeOfRawData == 0 || rawData == NULL){
        throw "no Data!";
    }
    m_encode = true;
    auto startTime = system_clock::now();
    m_sizeOfRawData = sizeOfRawData;
    m_rawData = new char[m_sizeOfRawData];
    map<char, int> analyzeMap;
    multimap<double, char> frequencyMap;
    HuffmanNode *rootNodes, *tempNode, *nodes;
    int counter = 0, numberOfBasicNodes, currentNumberOfRootNodes = 0, rootCounter = 0, bytes = 0;
    string currentCode;
    bool swapped;
    char byte;

    //copying rawdata
    for (int i = 0; i < m_sizeOfRawData; ++i) {
        m_rawData[i] = rawData[i];
    }

    //analysing rawdata
    for (int i = 0; i < m_sizeOfRawData; ++i) {
        analyzeMap[rawData[i]]++;
    }

    //calculating frequenzies
    for(map<char, int>::iterator iter = analyzeMap.begin(); iter != analyzeMap.end(); iter++){
        frequencyMap.insert(make_pair((double)iter->second/(double)m_sizeOfRawData, iter->first));
    }

    numberOfBasicNodes = analyzeMap.size();
    nodes = new HuffmanNode[numberOfBasicNodes];
    rootNodes = new HuffmanNode[numberOfBasicNodes-1];
    for(map<double, char>::iterator iter = frequencyMap.begin(); iter != frequencyMap.end(); iter++){
//        cout << iter->second << " => " << iter->first << endl;
        nodes[counter] = HuffmanNode(iter->second, iter->first);
        counter++;
    }

    //sort nodes
    do{
        swapped = false;
        for (int i = 0; i < numberOfBasicNodes-1; ++i) {
            if(nodes[i].getProbability() > nodes[i+1].getProbability()){
                HuffmanNode temp = HuffmanNode(nodes[i]);
                nodes[i] = nodes[i+1];
                nodes[i+1] = temp;
                swapped = true;
            }
        }
    }while(swapped);

    counter = 0;
    //build huffmantree
    while(currentNumberOfRootNodes < numberOfBasicNodes-1){
        double probability = 0;
        HuffmanNode newNode;
        if(currentNumberOfRootNodes == 0){
            probability = nodes[counter].getProbability() + nodes[counter+1].getProbability();
            counter +=2;
            newNode = HuffmanNode('\0', probability);
            newNode.setLeft(&nodes[counter-2]);
            newNode.setRight(&nodes[counter-1]);
            rootNodes[currentNumberOfRootNodes] = newNode;
            nodes[counter-2].setRoot(&rootNodes[currentNumberOfRootNodes]);
            nodes[counter-2].setFlag(false);
            nodes[counter-1].setRoot(&rootNodes[currentNumberOfRootNodes]);
            nodes[counter-1].setFlag(true);
            currentNumberOfRootNodes++;
        }else{
            if(counter < numberOfBasicNodes){
                if(nodes[counter].getProbability()>=rootNodes[rootCounter].getProbability()){
                        probability += rootNodes[rootCounter].getProbability();
                        rootCounter++;
                        if(rootCounter >= currentNumberOfRootNodes){
                            probability += nodes[counter].getProbability();
                            counter++;
                            newNode = HuffmanNode('\0', probability);
                            newNode.setLeft(&rootNodes[rootCounter-1]);
                            newNode.setRight(&nodes[counter-1]);
                            rootNodes[currentNumberOfRootNodes] = newNode;
                            rootNodes[rootCounter-1].setRoot(&rootNodes[currentNumberOfRootNodes]);
                            rootNodes[rootCounter-1].setFlag(false);
                            nodes[counter-1].setRoot(&rootNodes[currentNumberOfRootNodes]);
                            nodes[counter-1].setFlag(true);
                            currentNumberOfRootNodes++;
                        }else{
                            if(nodes[counter].getProbability()>=rootNodes[rootCounter].getProbability()){
                                probability += rootNodes[rootCounter].getProbability();
                                rootCounter++;
                                newNode = HuffmanNode('\0', probability);
                                newNode.setLeft(&rootNodes[rootCounter-2]);
                                newNode.setRight(&rootNodes[rootCounter-1]);
                                rootNodes[currentNumberOfRootNodes] = newNode;
                                rootNodes[rootCounter-2].setRoot(&rootNodes[currentNumberOfRootNodes]);
                                rootNodes[rootCounter-2].setFlag(false);
                                rootNodes[rootCounter-1].setRoot(&rootNodes[currentNumberOfRootNodes]);
                                rootNodes[rootCounter-1].setFlag(true);
                                currentNumberOfRootNodes++;
                            }else{
                                probability += nodes[counter].getProbability();
                                counter++;
                                newNode = HuffmanNode('\0', probability);
                                newNode.setLeft(&rootNodes[rootCounter-1]);
                                newNode.setRight(&nodes[counter-1]);
                                rootNodes[currentNumberOfRootNodes] = newNode;
                                rootNodes[rootCounter-1].setRoot(&rootNodes[currentNumberOfRootNodes]);
                                rootNodes[rootCounter-1].setFlag(false);
                                nodes[counter-1].setRoot(&rootNodes[currentNumberOfRootNodes]);
                                nodes[counter-1].setFlag(true);
                                currentNumberOfRootNodes++;
                            }
                        }
                }else{
                    probability += nodes[counter].getProbability();
                    counter++;
                    if(nodes[counter].getProbability()>=rootNodes[rootCounter].getProbability()){
                        probability += rootNodes[rootCounter].getProbability();
                        rootCounter++;
                        newNode = HuffmanNode('\0', probability);
                        newNode.setLeft(&nodes[counter-1]);
                        newNode.setRight(&rootNodes[rootCounter-1]);
                        rootNodes[currentNumberOfRootNodes] = newNode;
                        nodes[counter-1].setRoot(&rootNodes[currentNumberOfRootNodes]);
                        nodes[counter-1].setFlag(false);
                        rootNodes[rootCounter-1].setRoot(&rootNodes[currentNumberOfRootNodes]);
                        rootNodes[rootCounter-1].setFlag(true);
                        currentNumberOfRootNodes++;
                    }else{
                        probability += nodes[counter].getProbability();
                        counter++;
                        newNode = HuffmanNode('\0', probability);
                        newNode.setLeft(&nodes[counter-2]);
                        newNode.setRight(&nodes[counter-1]);
                        rootNodes[currentNumberOfRootNodes] = newNode;
                        nodes[counter-2].setRoot(&rootNodes[currentNumberOfRootNodes]);
                        nodes[counter-2].setFlag(false);
                        nodes[counter-1].setRoot(&rootNodes[currentNumberOfRootNodes]);
                        nodes[counter-1].setFlag(true);
                        currentNumberOfRootNodes++;
                    }
                }
            }else{
                probability = rootNodes[rootCounter].getProbability() + rootNodes[rootCounter+1].getProbability();
                rootCounter +=2;
                newNode = HuffmanNode('\0', probability);
                newNode.setLeft(&rootNodes[rootCounter-2]);
                newNode.setRight(&rootNodes[rootCounter-1]);
                rootNodes[currentNumberOfRootNodes] = newNode;
                rootNodes[rootCounter-2].setRoot(&rootNodes[currentNumberOfRootNodes]);
                rootNodes[rootCounter-2].setFlag(false);
                rootNodes[rootCounter-1].setRoot(&rootNodes[currentNumberOfRootNodes]);
                rootNodes[rootCounter-1].setFlag(true);
                currentNumberOfRootNodes++;
            }
        }
    }

    //create codeMap
    map<char, string>codeMap;
    for (int i = 0; i < numberOfBasicNodes; ++i) {
        tempNode = &nodes[i];
        byte = tempNode->getValue();
        currentCode = "";
//        cout   << byte << " => ";
        while (&tempNode->getRoot()!=NULL) {
//            cout << (tempNode->getFlag()==true?"I":"0");
            currentCode += (tempNode->getFlag()==true?"I":"0");
            tempNode = &tempNode->getRoot();
        }
        codeMap[byte] = invert(currentCode);
//        cout << currentCode << endl;
    }

    //print codes
//    for(map<char, string>::iterator iter = codeMap.begin(); iter != codeMap.end(); iter++){
//        cout << iter->first << " => " << iter->second << endl;
//    }

    for (int i = 0; i < m_sizeOfRawData; ++i) {
        m_sizeOfCompressedData += codeMap[m_rawData[i]].size();
    }

    cout << "sizeOfCompressData: " << m_sizeOfCompressedData << endl;

    //compress rawdata
    bytes = m_sizeOfCompressedData%8==0?m_sizeOfCompressedData/8:m_sizeOfCompressedData/8+1;
    m_compressedData = new char[m_sizeOfCompressedData];
    int sizeOfCode, overflow = 0, byteCounter = 0;
    for (int i = 0; i < m_sizeOfRawData; ++i) {
        currentCode = codeMap[m_rawData[i]];
        sizeOfCode = currentCode.size();
        for (int j = 0; j < sizeOfCode; ++j) {
            if(overflow == 8){
                byteCounter++;
                overflow = 0;
            }
            m_compressedData[byteCounter] <<= 1;
            if(currentCode[j]=='I'){
                m_compressedData[byteCounter] += 1;
            }
            overflow++;
        }
    }
    for (int i = 0; i < bytes*8-m_sizeOfCompressedData; ++i) {
        m_compressedData[bytes-1] <<= 1;
    }

    m_compressionRate = (double)m_sizeOfRawData*8/m_sizeOfCompressedData;

//    cout << "\n===================================\n";
//    cout << "size of rawdata: " << m_sizeOfRawData*8 << " bits";
//    cout << " size of compressed data: " << m_sizeOfCompressedData << " bits";
//    cout << " compression rate: " << m_compressionRate;
//    cout << "\n===================================\n";

    m_sizeOfCodeTable = codeMap.size()*2;
    for(map<char, string>::iterator iter = codeMap.begin(); iter != codeMap.end(); iter++){
        m_sizeOfCodeTable += iter->second.size()%8==0?iter->second.size()/8:iter->second.size()/8+1;
    }
    m_codeTable = new char[m_sizeOfCodeTable];
    counter = 0;
    for(map<char, string>::iterator iter = codeMap.begin(); iter != codeMap.end(); iter++){
        m_codeTable[counter] = iter->first;
        counter++;
        currentCode = iter->second;
        m_codeTable[counter] = currentCode.size();
        counter++;
        bytes = currentCode.size()%8==0?currentCode.size()/8:currentCode.size()/8+1;
        for (int i = 0; i < currentCode.size(); ++i) {
            if(overflow == 8){
                overflow = 0;
                counter++;
            }
            m_codeTable[counter] <<= 1;
            if(currentCode[i]=='I'){
                m_codeTable[counter] += 1;
            }
        }
        for (int i = 0; i < bytes*8-currentCode.size(); ++i) {
            m_codeTable[counter] <<= 1;
        }
        counter++;
    }

    //print codeTable
//    for (int i = 0; i < m_sizeOfCodeTable; i+=3) {
//        cout << "value: " << m_codeTable[i] << " size: " << (int)m_codeTable[i+1] << " code: ";
//        int c = 0;
//        for (int j = 1<<7; j > 0; j/=2) {
//           if(c==m_codeTable[i+1])
//                break;
//            m_codeTable[i+2]&j?printf("I"):printf("0");
//            c++;
//        }
//        cout << endl;
//    }

    nanoseconds time = system_clock::now()-startTime;
    m_compressionTime = time.count();

    return m_compressedData;
}

char* Huffman::decode(int sizeOfCompressedData, char *compressedData, int sizeOffCodeTable, char *codeTable){
    if(sizeOfCompressedData == 0 || compressedData== NULL){
        throw "no Data!";
    }
    m_encode = false;
    auto startTime = system_clock::now();
    map<string, char> codeMap;
    string currentCode;
    vector<char> uncompressedData;
    int sizeCounter, sizeOfCode;
    m_sizeOfCodeTable = sizeOffCodeTable;
    m_sizeOfCompressedData = sizeOfCompressedData;

    //create codeTable
    cout << endl;
    for (int i = 0; i < m_sizeOfCodeTable; i+=3) {
        sizeCounter = 0;
        currentCode = "";
        sizeOfCode = codeTable[i+1];
        for (int j = 1<<7; j > 0; j/=2) {
            if(sizeCounter==sizeOfCode)
                break;
            currentCode+=codeTable[i+2]&j?"I":"0";
            sizeCounter++;
        }
        codeMap[currentCode] = codeTable[i];
    }

//    for(map<string, char>::iterator iter = codeMap.begin(); iter != codeMap.end(); iter++){
//        cout << iter->second << " => " << iter->first << endl;
//    }

    //uncompress data
    currentCode = "";
    map<string, char>::iterator iter;
    sizeOfCompressedData = sizeOfCompressedData%8==0?sizeOfCompressedData/8:sizeOfCompressedData/8+1;
    for (int i = 0; i < sizeOfCompressedData; ++i) {
        for (int j = 1<<7; j > 0; j/=2) {
            currentCode += compressedData[i]&j?"I":"0";
            iter = codeMap.find(currentCode);
            if(iter != codeMap.end()){
                uncompressedData.push_back(codeMap[currentCode]);
                currentCode = "";
            }
        }
    }

    m_sizeOfRawData = uncompressedData.size();
    m_rawData = new char[m_sizeOfRawData];
    for (int i = 0; i < m_sizeOfRawData; ++i) {
        m_rawData[i] = uncompressedData[i];
    }
    cout << endl;

    nanoseconds time = system_clock::now()-startTime;
    m_compressionTime = time.count();

    return m_compressedData;
}

char* Huffman::getCodeTable(){
    return m_codeTable;
}

int Huffman::getSizeOfCodeTable(){
    return m_sizeOfCodeTable;
}

int Huffman::getSizeOfCompressedData(){
    return m_sizeOfCompressedData;
}

int Huffman::getSizeOfCompressedDataInBytes(){
    return m_sizeOfCompressedData%8==0?m_sizeOfCompressedData/8:m_sizeOfCompressedData/8+1;
}

int Huffman::getSizeOfRawData(){
    return m_sizeOfRawData;
}

int Huffman::getSizeOfRawDataInBytes(){
    return m_sizeOfRawData%8==0?m_sizeOfRawData/8:m_sizeOfRawData/8+1;
}

string Huffman::getStatistics(){
    string statistics = "";

    stringstream mystream;
//    mystream << "\n======================================\n";
    mystream << (m_encode == true?"\n\nKomprimieren:\n\n":"\n\nDekomprimieren:\n\n");
    mystream << "Größe der Rohdaten: " << m_sizeOfRawData*8 << " bits\n";
    mystream << "Größe der komprimierten Daten: " << m_sizeOfCompressedData << " bits\n";
    mystream << "Kompressionsrate: " << m_compressionRate;
    long compressionTime = m_compressionTime;
    long nanoSeconds = compressionTime%1000;
    compressionTime/= 1000;
    long microseconds = compressionTime%1000;
    compressionTime/= 1000;
    long miliseconds = compressionTime%1000;
    compressionTime/= 1000;
    long seconds = compressionTime;
    mystream << "\nAusführungszeit: ";
    mystream << seconds << "s " << miliseconds << "ms " << microseconds << "µs " << nanoSeconds << "ns" << endl;
//    mystream << "\n======================================\n";

    statistics = mystream.str();

    return statistics;
}

void Huffman::printStatistics(){
    cout << "\n==============================================================================================================================================\n";
    cout << (m_encode == true?"\n\nKomprimieren:\n\n":"\n\nDekomprimieren:\n\n");
    cout << "Größe der Rohdaten: " << m_sizeOfRawData*8 << " bits\n";
    cout << "Größe der komprimierten Daten: " << m_sizeOfCompressedData << " bits\n";
    cout << "Kompressionsrate: " << m_compressionRate;
    long compressionTime = m_compressionTime;
    long nanoSeconds = compressionTime%1000;
    compressionTime/= 1000;
    long microseconds = compressionTime%1000;
    compressionTime/= 1000;
    long miliseconds = compressionTime%1000;
    compressionTime/= 1000;
    long seconds = compressionTime;
    cout << "\nAusführungszeit: ";
    cout << seconds << "s " << miliseconds << "ms " << microseconds << "µs " << nanoSeconds << "ns" << endl;
    cout << "\n==============================================================================================================================================\n";

}

string invert(string code){
    string newCode = "";
    for (int i = code.size()-1; i >=0; i--) {
        newCode += code[i];
    }
    return newCode;
}
