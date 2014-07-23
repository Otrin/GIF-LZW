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


char* LZW::decode(unsigned char* rawData, int sizeRawData, char* alphabet, int sizeAlphabet, int mode, int countPixel)
{
    vector<CodeWord> table;
    char *pixel = new char[countPixel];
    int posPixel = 0;
    for(int i = 0; i<sizeAlphabet; i++){
        table.push_back(i);
    }
    int clearCode = sizeAlphabet;
    int endCode = sizeAlphabet+1;
    table.push_back(0);
    table.push_back(0);
    vector<CodeWord> tableBackup = table;
    unsigned int tableLength = sizeAlphabet+2;
    unsigned int tableLengthBackup = tableLength;
    int currentCodeSize = ((int)(log2(tableLength)))<(log2(tableLength))?(log2(tableLength))+1:(log2(tableLength));
    int startCodeSize = currentCodeSize;
    int currentBit = 0;
    int totalBits = sizeRawData*8;

    //for the start sequenze...
    unsigned int currentCode = getBits(rawData, currentBit, currentCodeSize);
    currentBit += currentCodeSize;

    //first code
    currentCode = getBits(rawData, currentBit, currentCodeSize);
    currentBit += currentCodeSize;
    unsigned int lastCode = currentCode;
    table.at(currentCode).getString(alphabet, pixel, posPixel);
    posPixel+=(table[currentCode].getSize()*3);
    while(currentBit+currentCodeSize<totalBits){
        if((unsigned int)zweiHochX2(currentCodeSize)-1 < tableLength && currentCodeSize < 12)
            currentCodeSize++;
        currentCode = getBits(rawData, currentBit, currentCodeSize);
        currentBit += currentCodeSize;
        if(currentCode == (unsigned int)clearCode){
            //clear table;
            table = tableBackup;
            tableLength = tableLengthBackup;
            currentCodeSize = startCodeSize;
            //first Code
            currentCode = getBits(rawData, currentBit, currentCodeSize);
            currentBit += currentCodeSize;
            lastCode = currentCode;
            table.at(currentCode).getString(alphabet, pixel, posPixel);
            posPixel+=(table.at(currentCode).getSize()*3);
        } else if(currentCode == (unsigned int)endCode){
            //exit
            break;
        } else {
            CodeWord tmp;
            if(currentCode < tableLength){
                tmp = (table[lastCode]); //lastCode in table + ...
                tmp.append(table[currentCode].getFirst()); // ...currentCode.First in table
                //table.append(tmp);
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
                table.at(currentCode).getString(alphabet, pixel, posPixel);
            } else {
                table.at(0).getString(alphabet, pixel, posPixel);
            }
            posPixel+=(table.at(currentCode).getSize()*3);
            lastCode = currentCode;
            //currentBit += currentCodeSize;
        }
    }
    return pixel;
}

unsigned char *LZW::encode(char *p_uncompData, int p_size, char *p_alphabet, int p_sizeAlphabet, int &p_codeSize)
{
    vector<unsigned char> compData;
    vector<CodeWord> table;
    vector<CodeWord> tableBackup;
    HashMap map;
    HashMap mapBackup;
    int sizeOfTable = 0;

    for(int i = 0; i<p_sizeAlphabet; ++i){
        table.push_back(i);
        map.put(table.at(i), i);
        sizeOfTable++;
    }
    int clearCode = p_sizeAlphabet;
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
    CodeWord indexBuffer(IO::getBit(p_uncompData[0],0,8)); //first char in IndexBuffer
    CodeWord nextIndexBuffer;
    int lastPosInTable = map.get(indexBuffer);
    for(int i = 1; i<p_size; ++i){
        if(i%100 == 0)
            cout << i << " von " << p_size << endl;
        if(exp2(currentCodeLength) < sizeOfTable)
            currentCodeLength++;
        k = IO::getBit(p_uncompData[i],0,8);
        nextIndexBuffer = indexBuffer;
        nextIndexBuffer.append(k);
        posInTable = map.get(nextIndexBuffer);
        if(posInTable >- 1){ //indexBuffer+k in table?
            indexBuffer = nextIndexBuffer;   //indexBuffer += k;
            lastPosInTable = posInTable;
        } else {
            int output = lastPosInTable;
//            cout << output << " , ";
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
    p_codeSize = compData.size();
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
