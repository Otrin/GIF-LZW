#include "lzw.h"
#include <iostream>
#include <math.h>
#include <string.h>
#include "codeword.h"
#include <io.h>
#include <QDebug>
#include <vector>
#include "hashmap.h"
using namespace std;

static int zweiHochX2(int x);
unsigned int LZW::getBits(const unsigned char *rawData, int pos, int currentCodeSize)
{
    int remaining = currentCodeSize;
    int index = pos/8;
    int place = pos%8;
    unsigned int codeWord = 0;
    int i = 0;
    while(remaining > 0){
        codeWord += (rawData[index] & (1<<place))?(1<<i):0;
        i++;
        remaining--;
        place++;
        if(place == 8){
            place = 0;
            index++;
        }

    }
    return codeWord;
}

void LZW::inCompData(int code, std::vector<unsigned char> &compData, int currentCodeLength, int currentBit)
{
    int index = currentBit/8;
    int pos = currentBit%8;
    int remainingBits = currentCodeLength;
    int bitsToReadAtThisIndex = 0;
    int data;
    while(remainingBits > 0){
        bitsToReadAtThisIndex = remainingBits>(8-pos)?8-pos:remainingBits;
        data = IO::getBit(code, currentCodeLength-remainingBits, bitsToReadAtThisIndex);
        if(pos > 0 ){
            data = IO::getBit(compData.at(index),0,8) | (data<<pos);
            compData.at(index) = (unsigned char) data;
        } else {
            compData.push_back((unsigned char) data);
        }
        remainingBits -= bitsToReadAtThisIndex;
        pos += bitsToReadAtThisIndex;
        if(pos>7){
            pos = 0;
            index++;
        }
    }
}

std::vector<int> LZW::tableContains(std::vector<CodeWord> table, CodeWord buffer, int k)
{
    std::vector<int> result;
    if(k != -1){
        buffer.append(k);
    }
    for(unsigned int i = 0; i<table.size(); ++i){
        if(buffer.similar(table.at(i)) == 1){
            result.push_back(i);
        }
    }
    return result;
}

std::vector<int> LZW::tableContains(std::vector<CodeWord> table, std::vector<int> lastPosInTable, CodeWord buffer, int k)
{
    std::vector<int> result;
    if(k != -1){
        buffer.append(k);
    }
    for(unsigned int i = 0; i<lastPosInTable.size(); ++i){
        if(buffer.similar(table.at(lastPosInTable.at(i))) == 1){
            result.push_back(lastPosInTable.at(i));
        }
    }
    return result;
}

void LZW::resetLastPosInTable(std::vector<int> &table, int n)
{
    table.clear();
    for(int i = 0; i<n; ++i){
        table.push_back(i);
    }
}

LZW::LZW()
{
}

unsigned char* LZW::decode(unsigned char* p_compData, int p_sizeOfCompData, unsigned char* p_codeTable, int p_sizeOfCodeTable)
{
    return NULL;
}

unsigned char* LZW::decode(Gif p_gif, int p_frame)
{
    if(p_gif.getFrame(p_frame)->getLctFlag() == 1){
        m_codeTable = p_gif.getFrame(p_frame)->getLct();
        m_sizeOfCodeTable = p_gif.getFrame(p_frame)->getSizeOfLCT();
    } else {
        m_codeTable = p_gif.getGCT();
        m_sizeOfCodeTable = p_gif.getSizeOfGCT();
    }
    vector<CodeWord> table;
    CodeWord ttt;
    ttt.append(1);
    m_sizeOfRawData = p_gif.getFrame(p_frame)->getHeight()*p_gif.getFrame(p_frame)->getWidth();
    m_rawData = new unsigned char[m_sizeOfRawData];
    m_sizeOfCompData = p_gif.getFrame(p_frame)->getSizeOfData();
    m_compData = p_gif.getFrame(p_frame)->getData();
    int posPixel = 0;
    for(int i = 0; i<m_sizeOfCodeTable; i++){
        table.push_back(i);
    }
    int clearCode = m_sizeOfCodeTable;
    int endCode = m_sizeOfCodeTable+1;
    table.push_back(clearCode);
    table.push_back(endCode);
    vector<CodeWord> tableBackup = table;
    unsigned int tableLength = table.size();
    unsigned int tableLengthBackup = tableLength;
    int currentCodeSize = ((int)(log2(tableLength)))<(log2(tableLength))?(log2(tableLength))+1:(log2(tableLength));
    int startCodeSize = currentCodeSize;
    int currentBit = 0;
    int totalBits = m_sizeOfCompData*8;

    //for the start sequenze...
    unsigned int currentCode = getBits(m_compData, currentBit, currentCodeSize);
    currentBit += currentCodeSize;

    //first code
    currentCode = getBits(m_compData, currentBit, currentCodeSize);
    currentBit += currentCodeSize;
    unsigned int lastCode = currentCode;
    table.at(currentCode).getString(m_rawData, posPixel);
    posPixel+=(table[currentCode].getSize()*3);
    while(currentBit+currentCodeSize<totalBits){
        if((unsigned int)zweiHochX2(currentCodeSize)-1 < tableLength && currentCodeSize < 12)
            currentCodeSize++;
        currentCode = getBits(m_compData, currentBit, currentCodeSize);
        currentBit += currentCodeSize;
        if(currentCode == (unsigned int)clearCode){
            //clear table;
            table = tableBackup;
            tableLength = tableLengthBackup;
            currentCodeSize = startCodeSize;
            //first Code
            currentCode = getBits(m_compData, currentBit, currentCodeSize);
            currentBit += currentCodeSize;
            lastCode = currentCode;
            table.at(currentCode).getString(m_rawData, posPixel);
            posPixel+=(table.at(currentCode).getSize()*3);
        } else if(currentCode == (unsigned int)endCode){
            //exit
            break;
        } else {
            CodeWord tmp;
            if(currentCode < tableLength){
                tmp = (table[lastCode]); //lastCode in table + ...
                tmp.append(table[currentCode].getFirst()); // ...currentCode.First in table
                table.push_back(tmp);
                tableLength++;
            } else {
                tmp = table.at(lastCode);
                tmp.append(table.at(lastCode).getFirst());
                //table.append(tmp);
                table.push_back(tmp);
                tableLength++;
            }
            if(currentCode < table.size()) {
                table.at(currentCode).getString(m_rawData, posPixel);
            } else {
                table.at(0).getString(m_rawData, posPixel);
            }
            posPixel+=(table.at(currentCode).getSize()*3);
            lastCode = currentCode;
            //currentBit += currentCodeSize;
        }
    }
    return m_rawData;
}

unsigned char *LZW::encode(Gif p_gif, int p_frame)
{

}

unsigned char *LZW::encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeOfCodeTable)
{
    vector<unsigned char> compData;
    vector<CodeWord> table;
    vector<CodeWord> tableBackup;
    HashMap map;
    HashMap mapBackup;
    int sizeOfTable = 0;

    for(int i = 0; i<p_sizeOfCodeTable; ++i){
        table.push_back(i);
        map.put(table.at(i), i);
        sizeOfTable++;
    }
    int clearCode = p_sizeOfCodeTable;
    int endCode = clearCode+1;
    table.push_back(clearCode); //clearCode
    table.push_back(endCode); //endCode
    tableBackup = table;
    mapBackup = map;
    sizeOfTable += 2;
    int sizeOfTableBackup = sizeOfTable;
    int k;
    int currentCodeLength = ((int)log2(sizeOfTable)<log2(sizeOfTable)?log2(sizeOfTable)+1:log2(sizeOfTable));
    int startCodeSize = currentCodeLength;
    int currentBit = 0;

    int posInTable;

    inCompData(clearCode, compData, currentCodeLength, currentBit); //clearCode in output
    currentBit += currentCodeLength;
    CodeWord indexBuffer(IO::getBit(p_rawData[0],0,8)); //first char in IndexBuffer
    CodeWord nextIndexBuffer;
    int lastPosInTable = map.get(indexBuffer);
    for(int i = 1; i<p_sizeOfRawData; ++i){
        if(i%100 == 0)
            cout << i << " von " << p_sizeOfRawData << endl;
        if(exp2(currentCodeLength) < sizeOfTable)
            currentCodeLength++;
        k = IO::getBit(p_rawData[i],0,8);
        nextIndexBuffer = indexBuffer;
        nextIndexBuffer.append(k);
        posInTable = map.get(nextIndexBuffer);
        if(posInTable >- 1){ //indexBuffer+k in table?
            indexBuffer = nextIndexBuffer;   //indexBuffer += k;
            lastPosInTable = posInTable;
        } else {
            int output = lastPosInTable;
            cout << output << " , ";
            inCompData(output, compData, currentCodeLength, currentBit); //index of indexBuffer+k in table in the output.
            currentBit += currentCodeLength;
            indexBuffer.append(k);
            table.push_back(indexBuffer); //indexBuffer+k in table
            map.put(indexBuffer, sizeOfTable);
            sizeOfTable++;
            indexBuffer = k;
            if(sizeOfTable >= 4096){
                cout << "clear------------------------" << endl;
                inCompData(clearCode, compData, currentCodeLength, currentBit);
                currentBit += currentCodeLength;
                table = tableBackup;
                map = mapBackup;
                sizeOfTable = sizeOfTableBackup;
                currentCodeLength = startCodeSize;
            }
        }
    }
    if(zweiHochX2(currentCodeLength) < sizeOfTable)
        currentCodeLength++;
    inCompData(tableContains(table, indexBuffer).at(0), compData, currentCodeLength, currentBit);
    currentBit += currentCodeLength;
    inCompData(endCode, compData, currentCodeLength, currentBit);
    currentBit += currentCodeLength;

    unsigned char* output = new unsigned char[compData.size()];
    m_sizeOfCompData = compData.size();
    cout << "size: " << compData.size() << endl;
    std::copy(compData.begin(), compData.end(), output);
    return output;
}

int zweiHochX2(int x){
    int res = 1;
    for(int i = 0; i<x; i++)
        res *= 2;
    return res;
}
