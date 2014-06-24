#ifndef CODEWORD_H
#define CODEWORD_H

class CodeWord
{
public:
    int size;
    int* list;
    CodeWord(int i);
    ~CodeWord();
    CodeWord(const CodeWord& cW);
    void append(int c);
    const int &operator [](int i) const;
    int &operator [](int i);
    CodeWord();
    char* getString(char *alphabet, char *pixel, int posPixel);
    int getFirst();
    CodeWord &operator =(const CodeWord &cW);
    CodeWord &operator =(int i);
};

#endif // CODEWORD_H
