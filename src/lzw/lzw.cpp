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

std::vector<CodeWord> LZW::getTable() const
{
    return m_table;
}

LZW::LZW()
{
    m_highlightingArray = NULL;
}

LZW::~LZW()
{
    delete[] m_highlightingArray;
}

unsigned char* LZW::decode(unsigned char* p_compData, int p_sizeOfCompData, unsigned char* p_codeTable, int p_sizeOfCodeTable)
{
    return NULL;
}

unsigned char* LZW::decode(Gif &p_gif, int p_frame)
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
    int posRawData = 0;
    for(int i = 0; i<m_sizeOfCodeTable; i++){
        table.push_back(i);
    }
    int clearCode = m_sizeOfCodeTable;
    int endCode = m_sizeOfCodeTable+1;
    table.push_back(clearCode);
    table.push_back(endCode);
    vector<CodeWord> tableBackup(table);
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
    table.at(currentCode).getString(m_rawData, posRawData);
    posRawData+=(table[currentCode].getSize());
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
            table.at(currentCode).getString(m_rawData, posRawData);
            posRawData+=(table.at(currentCode).getSize());
            cout << "-----------------clear-------" << endl;
        } else if(currentCode == (unsigned int)endCode){
            //exit
            break;
        } else {
            CodeWord tmp = table[lastCode]; //lastCode in table + ...
            if(currentCode < tableLength){
                tmp.append(table[currentCode].getFirst()); // ...currentCode.First in table
            } else {
                tmp.append(table.at(lastCode).getFirst()); // ...lastCode.First in table
            }
            table.push_back(tmp);
            tableLength++;
            table.at(currentCode).getString(m_rawData, posRawData);
            posRawData+=(table.at(currentCode).getSize());
            lastCode = currentCode;
        }
    }
    return m_rawData;
}

unsigned char *LZW::encode(Gif& p_gif, int p_frame)
{
    startEncode(p_gif, p_frame);
    for(m_i = 1; m_i<m_sizeOfRawData; ++m_i){
        nextStep();
    }
    endEncode(p_gif, p_frame);
    return m_compData;
}

void LZW::startEncode(Gif& p_gif, int p_frame)
{
    m_sizeOfRawData = p_gif.getFrame(p_frame)->getSizeOfData();
    m_rawData = p_gif.getFrame(p_frame)->getData();
    m_sizeOfTable = 0;

    if(p_gif.getFrame(p_frame)->getLctFlag() == 1){
        m_sizeOfCodeTable = p_gif.getFrame(p_frame)->getSizeOfLCT();
    } else {
        m_sizeOfCodeTable = p_gif.getSizeOfGCT();
    }
    for(int i = 0; i<m_sizeOfCodeTable; ++i){
        m_table.push_back(i);
        m_map.put(m_table.at(i), i);
        m_sizeOfTable++;
    }
    m_clearCode = m_sizeOfCodeTable;
    m_endCode = m_clearCode+1;
    m_table.push_back(m_clearCode); //clearCode
    m_table.push_back(m_endCode); //endCode
    m_tableBackup = m_table;
    m_mapBackup = m_map;
    m_sizeOfTable += 2;
    m_sizeOfTableBackup = m_sizeOfTable;
    m_currentCodeLength = ((int)log2(m_sizeOfTable)<log2(m_sizeOfTable)?log2(m_sizeOfTable)+1:log2(m_sizeOfTable));
    m_startCodeLength = m_currentCodeLength;
    m_currentBit = 0;

    inCompData(m_clearCode, m_currentCompData, m_currentCodeLength, m_currentBit); //clearCode in output
    m_currentBit += m_currentCodeLength;
    m_indexBuffer = CodeWord(IO::getBit(m_rawData[0],0,8)); //first char in IndexBuffer
    m_lastPosInTable = m_map.get(m_indexBuffer);
}

void LZW::nextStep()
{
    if(m_i%100 == 0)
        cout << m_i << " von " << m_sizeOfRawData << endl;
    if(exp2(m_currentCodeLength) < m_sizeOfTable)
        m_currentCodeLength++;
    m_k = IO::getBit(m_rawData[m_i],0,8);
    m_nextIndexBuffer = m_indexBuffer;
    m_nextIndexBuffer.append(m_k);
    m_posInTable = m_map.get(m_nextIndexBuffer);
    if(m_posInTable >- 1){ //indexBuffer+k in table?
        m_indexBuffer = m_nextIndexBuffer;   //indexBuffer += k;
        m_lastPosInTable = m_posInTable;
    } else {
        int output = m_lastPosInTable;
        inCompData(output, m_currentCompData, m_currentCodeLength, m_currentBit); //index of indexBuffer+k in table in the output.
        m_currentBit += m_currentCodeLength;
        m_indexBuffer.append(m_k);
        m_table.push_back(m_indexBuffer); //indexBuffer+k in table
        m_map.put(m_indexBuffer, m_sizeOfTable);
        m_sizeOfTable++;
        m_indexBuffer = m_k;
        if(m_sizeOfTable >= 4096){
            cout << "clear------------------------" << endl;
            inCompData(m_clearCode, m_currentCompData, m_currentCodeLength, m_currentBit);
            m_currentBit += m_currentCodeLength;
            m_table = m_tableBackup;
            m_map = m_mapBackup;
            m_sizeOfTable = m_sizeOfTableBackup;
            m_currentCodeLength = m_startCodeLength;
        }
    }
}

void LZW::endEncode(Gif &p_gif, int p_frame)
{
    if(zweiHochX2(m_currentCodeLength) < m_sizeOfTable)
        m_currentCodeLength++;
    inCompData(m_map.get(m_indexBuffer), m_currentCompData, m_currentCodeLength, m_currentBit);
    m_currentBit += m_currentCodeLength;
    inCompData(m_endCode, m_currentCompData, m_currentCodeLength, m_currentBit);
    m_currentBit += m_currentCodeLength;
    m_sizeOfCompData = m_currentCompData.size();
    m_compData = new unsigned char[m_sizeOfCompData];
    std::copy(m_currentCompData.begin(), m_currentCompData.end(), m_compData);
    p_gif.getFrame(p_frame)->setData(m_currentCompData);
    p_gif.getFrame(p_frame)->setDataFlag(1);
}

unsigned char *LZW::encode(unsigned char *p_rawData, int p_sizeOfRawData, int p_sizeOfCodeTable)
{
    return NULL;
}

int zweiHochX2(int x){
    return 2<<(x-1);
}
