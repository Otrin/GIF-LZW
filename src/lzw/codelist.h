
#ifndef CODELIST_H
#define CODELIST_H
#include "codeword.h"

class CodeList
{
public:
    int size;
    CodeList(int i);
    CodeList(const CodeList& cL);
    CodeList();
    CodeWord* words;
    void append(CodeWord c);
    const CodeWord &operator [](int i) const;
    CodeWord &operator [](int i);
    int getFirst(int i);
    CodeList &operator =(const CodeList &cL);
};

#endif // CODELIST_H
