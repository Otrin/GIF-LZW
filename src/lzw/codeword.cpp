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

int CodeWord::getLast()
{
    return words.at(words.size()-1);
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

int CodeWord::equals(CodeWord cW)
{
    if(size == cW.size){
        for(int i = 0; i<size; ++i){
           if(words[i] != cW[i])
               return 0;
        }
        return 1;
    }
    return 0;
}

int CodeWord::numberInTable(vector<CodeWord> table, vector<int> posInTable)
{
    for(unsigned int i= 0; i<posInTable.size(); ++i){
        if(equals(table.at(posInTable.at(i))) == 1){
            return posInTable.at(i);
        }
    }
}

int CodeWord::similar(CodeWord cW)
{
    if(size <= cW.size){
        for(int i = 0; i<size; ++i){
           if(words[i] != cW[i])
               return 0;
        }
        return 1;
    }
    return 0;
}

void CodeWord::print(){
    for(int i = 0; i<size; ++i){
        cout << words.at(i) << ",";
    }
    cout << endl;
}
