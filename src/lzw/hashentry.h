#ifndef HASHENTRY_H
#define HASHENTRY_H

#include "codeword.h"

class HashEntry {

private:
    CodeWord key;
    int value;
public:
    HashEntry(CodeWord key, int value) {
        this->key = key;
        this->value = value;
    }
    CodeWord* getKey() {
        return &key;
    }
    int getValue() {
        return value;
    }
};

#endif // HASHENTRY_H
