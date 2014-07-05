#ifndef IO_H
#define IO_H
#include "gif.h"
#include <fstream>
#include <iostream>
#include <vector>


class IO
{
    void decompress(int img);
    std::string m_fileName;
    int m_gce, m_par, m_pte, m_appEx, m_commEx;
    char *m_fileContent;
    char *m_colorTable;
    char *m_uncompCodeTable;
    char* m_output;
    char* m_lctTable;
    char* m_gctTable;
    unsigned char* m_codeTable;
    Gif gif;
    void setHeader(char* p_output, int& p_pointer);
    void getScreen(int& p_pointer);
    void setScreen(char* p_output, int& p_pointer);
    void getLCT(int &p_pointer, int p_frame);
    void setLCT(char* p_output, int& p_pointer, int p_frame);
    void getGCT(int &p_pointer);
    void setGCT(char* p_output, int& p_pointer);
    void getGCE(int &p_pointer, int p_frame);
    void setGCE(char* p_output, int& p_pointer, int p_frame);
    void getIDiscr(int &p_pointer, int p_frame);
    void setIDiscr(char* p_output, int& p_pointer, int p_frame);
    void getIData(int &p_pointer, int p_frame);
    void setIData(char* p_output, int& p_pointer, int p_frame);
    void getPTE(int &pointer);
    void getAE(int &pointer);
    void getCE(int &pointer);
    void getTrailer(int &pointer);
    void setTrailer(char* p_output, int& p_pointer);
    unsigned int getNextByte(int &p_pointer);
    void getFile(char *p_fileName, char *p_content, int p_size);
    void saveFile(char *p_fileName, char *p_output, int p_fileSize);
    void generateColorTable(Gif p_gif, int p_frame, std::vector<char> &p_codeTable);

public:
    IO();
    ~IO();
    IO(std::string p_filePath);
    void loadFile();
    void generateFile();
    Gif* getGif();
    static int getBit(int wert, int bit, int p_count);
    static int zweiHochX(int p_exponent);
    static char getHex(int p_binary);
    static unsigned char getBinChar(unsigned int p_binary);
};

#endif // IO_H
