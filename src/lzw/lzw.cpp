#include "lzw.h"
#include <iostream>
#include <math.h>
#include <string.h>
#include "codelist.h"
#include <io.h>
#include <QDebug>
#include <vector>
using namespace std;

static int zweiHochX2(int x);
static void inTable(CodeList table, unsigned int &tableLength, CodeWord cL);
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

LZW::LZW()
{
}


char* LZW::decode(unsigned char* rawData, int sizeRawData, char* alphabet, int sizeAlphabet, int mode, int countPixel)
{
    //CodeList table(sizeAlphabet+2);
    vector<CodeWord> tableL;
    char *pixel = new char[countPixel];
    int posPixel = 0;
    for(int i = 0; i<sizeAlphabet; i++){
        //table[i] = i;
        tableL.push_back(i);
    }
    int clearCode = sizeAlphabet;
    int endCode = sizeAlphabet+1;
    //table[clearCode] = 0;
    tableL.push_back(0);
    //table[endCode] = 0;
    tableL.push_back(0);
    //CodeList tableBackup = table;
    vector<CodeWord> tableBackupL = tableL;
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
    tableL[currentCode].getString(alphabet, pixel, posPixel);
    posPixel+=(tableL[currentCode].size*3);
    while(currentBit+currentCodeSize<totalBits){
        if((unsigned int)zweiHochX2(currentCodeSize)-1 < tableLength && currentCodeSize < 12)
            currentCodeSize++;
        currentCode = getBits(rawData, currentBit, currentCodeSize);
        currentBit += currentCodeSize;
        //cout << "code: " << currentCode << " codeLength: " << currentCodeSize << " pixel: " << posPixel << " tableLength: " << tableLength << endl;
        if(currentCode == (unsigned int)clearCode){
            cout << "code: " << currentCode << endl;
            //clearTable(table);
            //table = tableBackup;
            tableL = tableBackupL;
            tableLength = tableLengthBackup;
            currentCodeSize = startCodeSize;
            //first Code
            currentCode = getBits(rawData, currentBit, currentCodeSize);
            currentBit += currentCodeSize;
            lastCode = currentCode;
            tableL[currentCode].getString(alphabet, pixel, posPixel);
            posPixel+=(tableL[currentCode].size*3);
            cout << "code: " << currentCode << endl;
            cout << "tablelength: " << tableLength << endl;
            cout << "clear!!!!!!!!!!!!!!!!!!!!!!!!!!! bei pixel: " << posPixel << endl;
        } else if(currentCode == (unsigned int)endCode){
            //exit
            cout << "exit" << endl;
            break;
        } else {
            CodeWord tmp;
            if(currentCode < tableLength){
                tmp = (tableL[lastCode]); //lastCode in table + ...
                tmp.append(tableL[currentCode].getFirst()); // ...currentCode.First in table
                //table.append(tmp);
                tableL.push_back(tmp);
                tableLength++;
            } else {
                tmp = (tableL[lastCode]);
                tmp.append(tableL[lastCode].getFirst());
                //table.append(tmp);
                tableL.push_back(tmp);
                tableLength++;
            }
            tableL[currentCode].getString(alphabet, pixel, posPixel);
            posPixel+=(tableL[currentCode].size*3);
            lastCode = currentCode;
            //currentBit += currentCodeSize;
        }
    }
    return pixel;
}

unsigned char *LZW::encode(char *compData, int size, int codeSize)
{
    return NULL;
}

int zweiHochX2(int x){
    int res = 1;
    for(int i = 0; i<x; i++)
        res *= 2;
    return res;
}

void inTable(CodeList *table, unsigned int &tableLength, CodeList cL){
    CodeList *tmp = new CodeList(tableLength+1);
    for(unsigned int i = 0; i< tableLength; i++){
        tmp[i] = table[i];
    }
    tmp[tableLength] = cL;
    tableLength++;
}
