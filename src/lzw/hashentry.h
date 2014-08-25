#ifndef HASHENTRY_H
#define HASHENTRY_H

#include "codeword.h"

/**
 * @brief
 *
 */
class HashEntry {

private:
    CodeWord key; /**< TODO */
    int value; /**< TODO */
public:
    /**
     * @brief
     *
     * @param key
     * @param value
     */
    HashEntry(CodeWord key, int value) {
        this->key = key;
        this->value = value;
    }
    /**
     * @brief
     *
     * @return CodeWord
     */
    CodeWord* getKey() {
        return &key;
    }
    /**
     * @brief
     *
     * @return int
     */
    int getValue() {
        return value;
    }
};

#endif // HASHENTRY_H
