#ifndef HASHMAP_H
#define HASHMAP_H

#include "hashentry.h"
#include <iostream>

const int TABLE_SIZE = 10000; /**< TODO */

/**
 * @brief
 *
 */
class HashMap {
private:
    HashEntry **table; /**< TODO */
public:
    /**
     * @brief
     *
     */
    HashMap() {
        table = new HashEntry*[TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; ++i){
            table[i] = NULL;
        }
    }

    /**
     * @brief returns the map-entry at key
     *
     * @param key the key of the entry
     * @return int the entry-value
     */
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

    /**
     * @brief puts an entry in the map
     *
     * @param key the key of the entry
     * @param value the value of teh entry
     */
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

    /**
     * @brief
     *
     */
    ~HashMap() {
        delete[] table;
    }

    /**
     * @brief deletes all entries
     *
     */
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

    /**
     * @brief returns a hash for an entry-key
     *
     * @param key teh key of the entry
     * @param k number obn interations, the hits
     * @return unsigned int the hash for this entry
     */
    static unsigned int getHash(CodeWord key, int k = 0){
		unsigned int hash = 0;
		for(int i = 0; i<key.getSize(); ++i){
            hash += key[i]*(i+1);
        }
        hash = (hash + k) % TABLE_SIZE;
        return hash;
    }
    /**
     * @brief
     *
     * @param hM
     * @return HashMap &operator
     */
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
