#ifndef CODEWORD_H
#define CODEWORD_H
#include <vector>
#include <QString>
/**
 * @brief
 *
 */
class CodeWord
{
    int size; /**< length of the sequenze */
    std::vector<int> words; /**< sequenze */
public:
	CodeWord();
    /**
     * @brief
     *
     * @param i
     */
    CodeWord(int i);
    /**
     * @brief
     *
     * @param cW
     */
    CodeWord(const CodeWord& cW);
    /**
     * @brief append an int to the codeword
     *
     * @param c an index to append to the codeword
     */
    void append(int c);
    /**
     * @brief
     *
     * @param i
     * @return const int &operator
     */
    const int &operator [](int i) const;
    /**
     * @brief
     *
     * @param i
     * @return int &operator
     */
    int &operator [](int i);
    /**
     * @brief writes the sequenze in the uncompressed-data
     *
     * @param p_rawData uncompressed data
     * @param p_posInRawData position to write
     */
    void getString(unsigned char *p_rawData, int p_posInRawData);
    /**
     * @brief returns the first int of the sequenze
     *
     * @return int
     */
    int getFirst();
    /**
     * @brief returns the last int of the sequenze
     *
     * @return int
     */
    int getLast();
    /**
     * @brief
     *
     * @param cW
     * @return CodeWord &operator
     */
    CodeWord &operator =(const CodeWord &cW);
    /**
     * @brief
     *
     * @param i
     * @return CodeWord &operator
     */
    CodeWord &operator =(int i);
    /**
     * @brief
     *
     * @return int
     */
    int getSize() const;
    /**
     * @brief compares two codewords, if they are the same sequenze
     *
     * @param cW othe codeword
     * @return int 1=equal, 0=not equal
     */
    int equals(CodeWord& cW);
    /**
     * @brief prints the sequenze on the console
     *
     */
    void print();
    /**
     * @brief returns, if two codewords are similar.
     *
     * they are similar, if this sequenze is the begin of another codeword
     *
     * @param cW
     * @return int
     */
    int similar(CodeWord &cW);
    /**
     * @brief returns the sequenze
     *
     * @return std::string the sequenze
     */
    std::string getSequenze();
};

#endif // CODEWORD_H
