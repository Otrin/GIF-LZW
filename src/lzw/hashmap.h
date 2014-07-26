#ifndef HASHMAP_H
#define HASHMAP_H

#include "hashentry.h"
#include <iostream>

const int TABLE_SIZE = 10000;

class HashMap {
private:
    HashEntry **table;
public:
    HashMap() {
        table = new HashEntry*[TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; ++i){
            table[i] = NULL;
        }
    }

    int get(CodeWord key) {
        int hash = getHash(key);
        int i = 0;
        while (table[hash] != NULL && table[hash]->getKey()->equals(key) == 0){
            hash = getHash(key, i++);
        }
        if (table[hash] == NULL) {
            return -1;
        } else {
            return table[hash]->getValue();
        }
    }

    void put(CodeWord key, int value) {
        unsigned int hash = getHash(key);
        int i = 0;
        while (table[hash] != NULL && table[hash]->getKey()->equals(key) == 0){
              hash = getHash(key, i++);
        }
        if (table[hash] != NULL){
            delete table[hash];
        }
        table[hash] = new HashEntry(key, value);
    }

    ~HashMap() {
        delete[] table;
    }

    void clear(){
        for (int i = 0; i < TABLE_SIZE; i++){
            if (table[i] != NULL){
                delete table[i];
            }
        }
        delete[] table;
        table = new HashEntry*[TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; i++){
            table[i] = NULL;
        }
    }

    static unsigned int getHash(CodeWord key, int k = 0){
        unsigned int hash;
        for(unsigned int i = 0; i<key.getSize(); ++i){
            hash += key[i]*(i+1);
        }
        hash = (hash + k) % TABLE_SIZE;
        return hash;
    }
    HashMap& operator=(const HashMap& hM){
        if(&hM != this){
            for(int i = 0; i<TABLE_SIZE; ++i){
                table[i] = hM.table[i];
            }
        }
		return *this;
    }
};

#endif // HASHMAP_H
