#include "codeword.h"
#include "io.h"
#include <iostream>

using namespace std;

int getBitW(int wert, int bit, int anz);
char getHexW(int i);
int zweiHochXW(int x);

CodeWord::CodeWord(int i)
{
    size = 1;
    list = new int[1];
    list[0] = i;
}

CodeWord::~CodeWord()
{
    delete[] list;
}

CodeWord::CodeWord(const CodeWord &cW)
{
    size = cW.size;
    list = new int[size];
    for(int i = 0; i<size; ++i){
        list[i] = cW[i];
    }
}

void CodeWord::append(int c)
{
    int *old = list;
    list = new int[size+1];
    for(int i = 0; i<size; ++i){
        list[i] = old[i];
    }
    delete[] old;
    list[size++] = c;
}

const int &CodeWord::operator [](int i) const
{
    return list[i];
}

int &CodeWord::operator [](int i)
{
    return list[i];
}

CodeWord::CodeWord()
{
    size = 1; //default init
    list = new int[1]; //default init
}

char* CodeWord::getString(char* alphabet, char* pixel, int posPixel)
{
    for(int i = 0; i<size; ++i){
       cout << "|" << list[i] << "|[";
       cout << IO::getBit((unsigned int)alphabet[list[i]*3], 0, 8) << ",";
       pixel[i+posPixel] = alphabet[list[i]*3];
       cout << IO::getBit((unsigned int)alphabet[list[i]*3+1], 0, 8) << ",";
       pixel[i+posPixel+1] = alphabet[list[i]*3+1];
       cout << IO::getBit((unsigned int)alphabet[list[i]*3+2], 0, 8) << "]";
       pixel[i+posPixel+2] = alphabet[list[i]*3+2];
    }
    cout << endl;
    return NULL;
}

int CodeWord::getFirst()
{
    return list[0];
}

CodeWord &CodeWord::operator =(const CodeWord &cW)
{
    if(&cW != this){
        size = cW.size;
        delete[] list;
        list = new int[size];
        for(int i = 0; i<size; ++i){
            list[i] = cW[i];
        }
    }
    return *this;
}

CodeWord &CodeWord::operator =(int i)
{
    size = 1;
    delete[] list;
    list = new int[1];
    list[0] = i;
    return *this;
}

int getBitW(int wert, int bit, int anz){
   return (wert & ((zweiHochXW(anz)-1) << bit))>>bit;
}

char getHexW(int i){
    if(i>9)
        return static_cast<char>(i+55);
    else
        return static_cast<char>(i+48);
}

int zweiHochXW(int x){
    int res = 1;
    for(int i = 0; i<x; i++)
        res *= 2;
    return res;
}
