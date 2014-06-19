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
    CodeWord lastOutput;
    cout << "code: " << currentCode << endl << "output: ";
    table[currentCode].getString(alphabet, pixel, posPixel);
    posPixel+=table[currentCode].size;

    while(currentBit+currentCodeSize<totalBits){
        if((unsigned int)zweiHochX2(currentCodeSize)-1 < tableLength)
            currentCodeSize++;
        currentCode = getBits(rawData, currentBit, currentCodeSize);
        cout << "code: " << currentCode << endl;
        if(currentCode == (unsigned int)clearCode){
            //clearTable(table);
            table = tableBackup;
            tableLength = tableLengthBackup;
            currentCodeSize = startCodeSize;
        } else if(currentCode == (unsigned int)endCode){
            //exit
            break;
        } else {
            if(currentCode < tableLength){
//                cout << "schon in tabelle" << endl;
            //current in output
//                cout << "output: ";
                table[currentCode].getString(alphabet, pixel, posPixel);
                posPixel += table[currentCode].size;
//                cout << endl;
            //lastOutput + currentOutput[first] in table
                CodeWord tmp = (table[lastCode]); //lastCode in table + ...
                tmp.append(table[currentCode].getFirst()); // ...currentCode.First in table
                table.append(tmp);
                tableLength++;
//                cout << "in table an stelle: " << tableLength-1 << " ";
//                tmp.getString();
//                cout << endl;
                lastOutput = tmp;
            } else {
//                cout << "noch nicht in tabelle" << endl;
                CodeWord tmp = (table[lastCode]);
                tmp.append(table[lastCode].getFirst());
                table.append(tmp);
                tableLength++;
//                cout << "output: ";
                tmp.getString(alphabet, pixel, posPixel);
                posPixel += table[currentCode].size;
//                cout << endl;
//                cout << "neu in table an stelle: " << tableLength-1 << " ";
//                tmp.getString();
//                cout << endl;
                lastOutput = tmp;
            }
        }
        lastCode = currentCode;
        currentBit += currentCodeSize;
    }
    return pixel;
}

char* LZW::encode(char *compData)
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
