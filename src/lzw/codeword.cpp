#include "codeword.h"
#include "io.h"
#include <iostream>
#include <vector>

using namespace std;

CodeWord::CodeWord(int i)
{
    size = 1;
    words.push_back(i);
}

CodeWord::~CodeWord()
{
}

CodeWord::CodeWord(const CodeWord &cW)
{
    size = cW.size;
    words.clear();
    for(int i = 0; i<size; ++i){
        words.push_back(cW[i]);
    }
 }

void CodeWord::append(int c)
{
    words.push_back(c);
    size++;
}

const int &CodeWord::operator [](int i) const
{
    return words.at(i);
}

int &CodeWord::operator [](int i)
{
    return words.at(i);
}

CodeWord::CodeWord()
{
}

char* CodeWord::getString(char* alphabet, char* pixel, int posPixel)
{
    for(int i = 0; i<size; ++i){
       pixel[posPixel++] = alphabet[words.at(i)*3];
       pixel[posPixel++] = alphabet[words.at(i)*3+1];
       pixel[posPixel++] = alphabet[words.at(i)*3+2];
    }

    return NULL;
}

int CodeWord::getFirst()
{
    return words.at(0);
}

CodeWord &CodeWord::operator =(const CodeWord &cW)
{
    if(&cW != this){
        size = cW.size;
        words.clear();
        for(int i = 0; i<size; ++i){
            words.push_back(cW[i]);
        }
    }
    return *this;
}

CodeWord &CodeWord::operator =(int i)
{
    size = 1;
    words.clear();
    words.push_back(i);
    return *this;
}

int CodeWord::getSize() const
{
    return words.size();
}
