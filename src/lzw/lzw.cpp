#include "lzw.h"
#include <iostream>
#include <math.h>
#include <string.h>
#include "codelist.h"
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
    CodeList table(sizeAlphabet+2);
    char *pixel = new char[countPixel];
    int posPixel = 0;
    for(int i = 0; i<sizeAlphabet; i++){
        table[i] = i;
    }
    int clearCode = sizeAlphabet;
    int endCode = sizeAlphabet+1;
    table[clearCode] = 0;
    table[endCode] = 0;
    CodeList tableBackup = table;
    unsigned int tableLength = sizeAlphabet+2;
    unsigned int tableLengthBackup = tableLength;
    int currentCodeSize = ((int)(log2(tableLength)))<(log2(tableLength))?(log2(tableLength))+1:(log2(tableLength));
    int startCodeSize = currentCodeSize;
    int currentBit = 0;
    int totalBits = sizeRawData*8;

    //for the start sequenze...
    unsigned int currentCode = getBits(rawData, currentBit, currentCodeSize);
    cout << "startcode: " << currentCode << endl;
    currentBit += currentCodeSize;

    //first code
    cout << "codeSize: " << currentCodeSize << endl;
    currentCode = getBits(rawData, currentBit, currentCodeSize);
    currentBit += currentCodeSize;
    unsigned int lastCode = currentCode;
    cout << "code: " << currentCode << endl;
    table[currentCode].getString(alphabet, pixel, posPixel);
    posPixel+=(table[currentCode].size*3);
    while(currentBit+currentCodeSize<totalBits){
        if((unsigned int)zweiHochX2(currentCodeSize)-1 < tableLength)
            currentCodeSize++;
        currentCode = getBits(rawData, currentBit, currentCodeSize);
        cout << "code: " << currentCode << " tablelength: " << tableLength << ", posPixel: " << posPixel << " von " << countPixel/3 << endl;
        if(currentCode == (unsigned int)clearCode){
            //clearTable(table);
            //table = tableBackup;
            tableLength = tableLengthBackup;
            currentCodeSize = startCodeSize;
            cout << "startcode: " << currentCode << endl;
            //start Code
            currentCode = getBits(rawData, currentBit, currentCodeSize);
            currentBit += currentCodeSize;
            //first Code
            currentCode = getBits(rawData, currentBit, currentCodeSize);
            currentBit += currentCodeSize;
            lastCode = currentCode;
            table[currentCode].getString(alphabet, pixel, posPixel);
            posPixel+=(table[currentCode].size*3);
            cout << "code: " << currentCode << endl;
            cout << "tablelength: " << tableLength << endl;
            cout << "clear!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        } else if(currentCode == (unsigned int)endCode){
            //exit
            break;
        } else {
            CodeWord tmp;
            if(currentCode < tableLength){
                tmp = (table[lastCode]); //lastCode in table + ...
                tmp.append(table[currentCode].getFirst()); // ...currentCode.First in table
                table.append(tmp);
            } else {
                tmp = (table[lastCode]);
                tmp.append(table[lastCode].getFirst());
                table.append(tmp);
            }
            table[currentCode].getString(alphabet, pixel, posPixel);
            tableLength++;
            posPixel+=(table[currentCode].size*3);
            lastCode = currentCode;
            currentBit += currentCodeSize;
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
    CodeList *tmp = new CodeList[tableLength+1];
    for(unsigned int i = 0; i< tableLength; i++){
        tmp[i] = table[i];
    }
    tmp[tableLength] = cL;
    tableLength++;
}
