#ifndef IO_H
#define IO_H
#include "gif.h"
#include "lzw.h"
#include <fstream>
#include <iostream>
#include <vector>


class IO
{
    void decompress(int p_frame);
    std::string m_fileName;
    int m_gce, m_par, m_pte, m_appEx, m_commEx;
    char *m_fileContent;
    unsigned char *m_rawData;
    char* m_output;
    unsigned char* m_LCT;
    unsigned char* m_GCT;
    unsigned char* m_compData;
    Gif gif;
    LZW m_lzw;
    void setHeader(char* p_output, int& p_pointer);
    void getScreen(int& p_pointer);
	void setScreen(Gif &p_gif, char* p_output, int& p_pointer);
    void getLCT(int &p_pointer, int p_frame);
	void setLCT(Gif &p_gif, char* p_output, int& p_pointer, int p_frame);
    void getGCT(int &p_pointer);
	void setGCT(Gif &p_gif, char* p_output, int& p_pointer);
    void getGCE(int &p_pointer, int p_frame);
	void setGCE(Gif& p_gif, char* p_output, int& p_pointer, int p_frame);
    void getIDiscr(int &p_pointer, int p_frame);
	void setIDiscr(Gif &p_gif, char* p_output, int& p_pointer, int p_frame);
    void getIData(int &p_pointer, int p_frame);
	void setIData(Gif &p_gif, char* p_output, int& p_pointer, int p_frame);
    void getPTE(int &pointer);
    void getAE(int &pointer);
    void getCE(int &pointer);
    void getTrailer(int &pointer);
    void setTrailer(char* p_output, int& p_pointer);
    unsigned int getNextByte(int &p_pointer);
    void getFile(char *p_fileName, char *p_content, int p_size);
	void saveFile(char *p_fileName, unsigned char *p_output, int p_fileSize);
    void getCommEx(int& pointer);
    void getAppEx(int &pointer);


	void prepareData(Gif& p_gif);
	void writeFrames(std::vector<unsigned char> &p_outputData, Gif& p_gif);
	void writeHeader(std::vector<unsigned char> &p_outputData, Gif& p_gif);
	void writeGCE(std::vector<unsigned char> &p_outputData, Gif& p_gif, int p_frame);
	void writeAppExt(std::vector<unsigned char> &p_outputData);
	void writeImageBlock(std::vector<unsigned char> &p_outputData, Gif& p_gif, int p_frame);
	void writeTrailer(std::vector<unsigned char> &p_outputData);



public:
    static void generateRawData(Gif &p_gif, int p_frame, bool p_withColorTable);
    static void generatePixel(Gif &p_gif, int p_frame);
    IO();
    ~IO();
    IO(std::string p_filePath);
    void loadFile();
    void generateFile();
	void saveGif(Gif &p_gif);
    Gif* getGif();
	void setGif(Gif &gif);
	static bool checkDelayTime(Gif &p_gif);
    static int getBit(int wert, int bit, int p_count);
    static int zweiHochX(int p_exponent);
    static char getHex(int p_binary);
    static unsigned char getBinChar(unsigned int p_binary);
};

#endif // IO_H
