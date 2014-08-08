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

CodeWord::CodeWord(const CodeWord &cW)
{
    size = cW.size;
    words = cW.words;
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
	size = 0;
}

void CodeWord::getString(unsigned char *p_rawData, int p_posInRawData)
{
    for(int i = 0; i<size; ++i){
       p_rawData[p_posInRawData++] = words.at(i);

    }
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
        words = cW.words;
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

int CodeWord::equals(CodeWord &cW)
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

int CodeWord::numberInTable(vector<CodeWord>& table, vector<int>& posInTable)
{
    for(unsigned int i= 0; i<posInTable.size(); ++i){
        if(equals(table.at(posInTable.at(i))) == 1){
            return posInTable.at(i);
        }
    }
    return -1;
}

QString &CodeWord::getSequenze()
{
    QString text;
    for (size_t i = 0; i < size; ++i) {
        text += words[i] + " ";
    }
    return text;
}

int CodeWord::similar(CodeWord& cW)
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
