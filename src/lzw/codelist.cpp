#include "codelist.h"
#include <iostream>
#include <stdio.h>
using namespace std;

CodeList::CodeList(int i){
    size = i;
    words = new CodeWord[size];
}

CodeList::CodeList(){
}

void CodeList::append(CodeWord c)
{
    CodeWord* old = words;
    words = new CodeWord[size+1];
    for(int i = 0; i<size; i++)
        words[i] = old[i];
    delete[] old;
    words[size++] = c;
}

const CodeWord &CodeList::operator [](int i) const {
    return words[i];
}

CodeWord &CodeList::operator [](int i){
    return words[i];
}

CodeList& CodeList::operator=(const CodeList& cL){
    if(&cL != this) {
        size = cL.size;
        delete[] words;
        words = new CodeWord[size];
        for (int i = 0; i<size; ++i){
            words[i] = cL[i];
        }
    }
    return *this;
}

int CodeList::getFirst(int i)
{
    return words[i].getFirst();
}
