#include "runlengthencoding.h"
#include <stdio.h>
#include <iostream>
#include <climits>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;

RunLengthEncoding::RunLengthEncoding(){
}

RunLengthEncoding::~RunLengthEncoding(){

}

unsigned char* RunLengthEncoding::encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeCodeTable){
    if(p_sizeOfRawData == 0 || p_rawData == NULL){
        throw "No Data!";
    }
    m_sizeOfRawData = p_sizeOfRawData;

    auto startTime = system_clock::now();
    unsigned char counter = 1, last = p_rawData[0];
    vector<unsigned char> compVector;
    for (int i = 1; i < p_sizeOfRawData; ++i) {
        if(last == p_rawData[i] && counter < UCHAR_MAX){
            counter++;
            last = p_rawData[i];
        }else{
            compVector.push_back(last);
            compVector.push_back(counter);
            last = p_rawData[i];
            counter = 1;
        }
    }

    compVector.push_back(last);
    compVector.push_back(counter);

    m_sizeOfCompData = compVector.size();
    m_compData = new unsigned char[m_sizeOfCompData];
    for (int i = 0; i < m_sizeOfCompData; ++i) {
        m_compData[i] = compVector[i];
    }

    nanoseconds time = system_clock::now() - startTime;
    m_timeAgo = time.count();

    return m_compData;
}

unsigned char* RunLengthEncoding::decode(unsigned char *p_compData, int p_sizeOfCompData, unsigned char *p_codeTable, int p_sizeOfCodeTable, int p_sizeOfOutput){
    if(p_sizeOfCompData == 0 || p_compData== NULL){
        throw "No Data!";
    }
    m_sizeOfCompData = p_sizeOfCompData;

    auto startTime = system_clock::now();
    unsigned char counter = 0;
    m_sizeOfRawData = 0;
    for (int i = 0; i < p_sizeOfCompData; i+=2) {
        m_sizeOfRawData += (int)p_compData[i+1];
    }

    m_rawData = new unsigned char[m_sizeOfRawData];
    for (int i = 0; i < m_sizeOfCompData; i+=2) {
        for (int j = 0; j < m_compData[j+1]; ++j) {
            m_rawData[counter] = m_compData[i];
            counter++;
        }
    }

    nanoseconds time = system_clock::now() - startTime;
    m_timeAgo = time.count();

    return m_rawData;
}
