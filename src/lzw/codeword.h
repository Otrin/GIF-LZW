#ifndef CODEWORD_H
#define CODEWORD_H
#include <vector>
class CodeWord
{
    int size;
    std::vector<int> words;
public:
    CodeWord(int i);
    ~CodeWord();
    CodeWord(const CodeWord& cW);
    void append(int c);
    const int &operator [](int i) const;
    int &operator [](int i);
    CodeWord();
    char* getString(char *alphabet, char *pixel, int posPixel);
    int getFirst();
    int getLast();
    CodeWord &operator =(const CodeWord &cW);
    CodeWord &operator =(int i);
    int getSize() const;
    int equals(CodeWord cW);
    void print();
    int similar(CodeWord cW);
    int numberInTable(std::vector<CodeWord> table, std::vector<int> posInTable);
};

#endif // CODEWORD_H
