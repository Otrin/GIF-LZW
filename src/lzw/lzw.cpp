#include "lzw.h"
#include <iostream>
#include <math.h>
#include <string.h>
#include "codeword.h"
#include <io.h>
#include <QDebug>
#include <vector>
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

void LZW::inCompData(int code, std::vector<unsigned char> compData, int currentCodeLength, int currentBit)
{

}

LZW::LZW()
{
}


char* LZW::decode(unsigned char* rawData, int sizeRawData, char* alphabet, int sizeAlphabet, int mode, int countPixel)
{
    //CodeList table(sizeAlphabet+2);
    vector<CodeWord> table;
    char *pixel = new char[countPixel];
    int posPixel = 0;
    for(int i = 0; i<sizeAlphabet; i++){
        //table[i] = i;
        table.push_back(i);
    }
    int clearCode = sizeAlphabet;
    int endCode = sizeAlphabet+1;
    //table[clearCode] = 0;
    table.push_back(0);
    //table[endCode] = 0;
    table.push_back(0);
    //CodeList tableBackup = table;
    vector<CodeWord> tableBackupL = table;
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
    //table[currentCode].getString(alphabet, pixel, posPixel);
    table[currentCode].getString(alphabet, pixel, posPixel);
    posPixel+=(table[currentCode].getSize()*3);
    while(currentBit+currentCodeSize<totalBits){
        if((unsigned int)zweiHochX2(currentCodeSize)-1 < tableLength && currentCodeSize < 12)
            currentCodeSize++;
        currentCode = getBits(rawData, currentBit, currentCodeSize);
        currentBit += currentCodeSize;
//        cout << "code: " << currentCode << " codeLength: " << currentCodeSize << " pixel: " << posPixel << " tableLength: " << tableLength << endl;
        if(currentCode == (unsigned int)clearCode){
//            cout << "code: " << currentCode << endl;
            //clearTable(table);
            //table = tableBackup;
            table = tableBackupL;
            tableLength = tableLengthBackup;
            currentCodeSize = startCodeSize;
            //first Code
            currentCode = getBits(rawData, currentBit, currentCodeSize);
            currentBit += currentCodeSize;
            lastCode = currentCode;
            table[currentCode].getString(alphabet, pixel, posPixel);
            posPixel+=(table[currentCode].getSize()*3);
//            cout << "code: " << currentCode << endl;
//            cout << "tablelength: " << tableLength << endl;
//            cout << "clear!!!!!!!!!!!!!!!!!!!!!!!!!!! bei pixel: " << posPixel << endl;
        } else if(currentCode == (unsigned int)endCode){
            //exit
//            cout << "exit" << endl;
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
                tmp = (table[lastCode]);
                tmp.append(table[lastCode].getFirst());
                //table.append(tmp);
                table.push_back(tmp);
                tableLength++;
            }
            table[currentCode].getString(alphabet, pixel, posPixel);

            posPixel+=(table[currentCode].getSize()*3);
            lastCode = currentCode;
            //currentBit += currentCodeSize;
        }
    }
    return pixel;
}

unsigned char *LZW::encode(char *p_uncompData, int p_size, char *p_alphabet, int p_sizeAlphabet, int p_codeSize)
{
    vector<unsigned char> compData;
    vector<CodeWord> table;

    for(int i = 0; i<p_sizeAlphabet; ++i){
        table.push_back(i);
    }
    table.push_back(0); //clearCode
    table.push_back(0); //endCode
    int k;
    int currentCodeLength = ((int)log2(p_sizeAlphabet)<log2(p_sizeAlphabet)?log2(p_sizeAlphabet)+1:log2(p_sizeAlphabet));
    int currentBit = 0;
    int clearCode = p_sizeAlphabet;
    int endCode = clearCode+1;
    inCompData(clearCode, compData, currentCodeLength, currentBit); //clearCode in output
    CodeWord indexBuffer(p_uncompData[0]); //first char in IndexBuffer
    for(int i = 1; i<p_size; ++i){
        k = p_uncompData[i];
        //if(indexBuffer+k in table?

            //yes -> //indexBuffer.append(k)

            //no ->  //table.push_back(indexBuffer+k);
                     //inCompData(index of indexBuffer in Table)
                     //indexBuffer = k
        //inCompData(indexBuffer)
        //inCompData(endCode)
    }
    unsigned char* output = new unsigned char[compData.size()];
    std::copy(compData.begin(), compData.end(), output);
    p_codeSize = compData.size();
    return output;
}

int zweiHochX2(int x){
    int res = 1;
    for(int i = 0; i<x; i++)
        res *= 2;
    return res;
}
