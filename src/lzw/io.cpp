#include "io.h"
#include "lzw.h"
#include "gif.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;



int get_size(const char *file);
int isColorInTable(char* pixel, int n, vector<char> color);

void IO::getScreen(int& p_pointer){
    //width of image
    int little = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    int big = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.setWidth((big<<8) + little);

    //height of image
    int big2 = 255 & (unsigned int)(m_fileContent[p_pointer+1]);
    int little2 = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.setHeight((big2<<8) + little2);

    //packed Bytea
    int packedByte = (unsigned int)(m_fileContent[(++p_pointer)++]);
    gif.setGctFlag(getBit(packedByte, 7, 1));
    gif.setSizeOfGCT(zweiHochX(1+getBit(packedByte, 0, 3)));

    //Background Color Image
    gif.setBgColor(getBit((unsigned int)(m_fileContent[p_pointer++]), 0, 8));

    //Pixel Aspect Ratio
    m_par = getBit((unsigned int)(m_fileContent[p_pointer++]),0,8);
}

void IO::setScreen(char *p_output, int &p_pointer)
{
    p_output[p_pointer++] = gif.getWidth()%256; //gif width (little endian)
    p_output[p_pointer++] = gif.getWidth() - (gif.getWidth()%256); //gif width
    p_output[p_pointer++] = gif.getHeight()%256; //gif height (little endian)
    p_output[p_pointer++] = gif.getHeight() - (gif.getHeight()%256); //gif height
    char gctFlag = 0;
    if(gif.getGctFlag())
        gctFlag = 1<<7;
    char colorRes = 1<<4;
    char sizeOfGCT = log2(gif.getSizeOfGCT()-2)-1;
    p_output[p_pointer++] = gctFlag & colorRes & sizeOfGCT; //packed Byte
    p_output[p_pointer++] = gif.getBgColor(); //Background color index
    p_output[p_pointer++] = 0; //Pixel Aspect Ratio
}

void IO::getLCT(int &p_pointer, int p_frame){
    char* table = new char[gif.getFrame(p_frame)->getSizeOfLCT()*3];
    for(int i = 0; i<gif.getFrame(p_frame)->getSizeOfLCT(); ++i){
        table[i*3] = m_fileContent[p_pointer++];
        table[i*3+1] = m_fileContent[p_pointer++];
        table[i*3+2] = m_fileContent[p_pointer++];
        gif.getFrame(p_frame)->setLct(table, gif.getFrame(p_frame)->getSizeOfLCT());
    }
}

void IO::setLCT(char *p_output, int &p_pointer, int p_frame)
{
    for(int i = 0; i<gif.getFrame(p_frame)->getSizeOfLCT()-2; ++i){ //last 2 codes not in file!
        p_output[p_pointer++] = gif.getFrame(p_frame)->getLct()[i]; //lines of Colors
    }
}

void IO::getGCT(int &p_pointer){
    char* table = new char[gif.getSizeOfGCT()*3];
    for(int i = 0; i<gif.getSizeOfGCT(); ++i){
        table[i*3] = m_fileContent[p_pointer++];
        table[i*3+1] = m_fileContent[p_pointer++];
        table[i*3+2] = m_fileContent[p_pointer++];
        gif.setColorTable(table, gif.getSizeOfGCT());
    }
}

void IO::setGCT(char *p_output, int &p_pointer)
{
    for(int i = 0; i<gif.getSizeOfGCT()-2; ++i){ //last 2 codes not in file!
        p_output[p_pointer++] = gif.getColorTable()[i]; //lines of Colors
    }
}

void IO::getGCE(int &p_pointer, int p_frame){ //Graphics Control Extension
    int blockSize = (unsigned int)m_fileContent[p_pointer++]; //unused, always 4!
    int packed = (unsigned int)m_fileContent[p_pointer++]; //packed Byte, the following three variables are the value
    gif.getFrame(p_frame)->setDisposualMethod((unsigned int)getBit(packed, 2, 3));
    gif.getFrame(p_frame)->setUserInputFlag((unsigned int)getBit(packed, 1, 1));
    gif.getFrame(p_frame)->setTranspColorFlag((unsigned int)getBit(packed, 0, 1));
    int little = 255 & ((unsigned int)(m_fileContent[p_pointer++]));
    int big = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.getFrame(p_frame)->setDelayTime((unsigned int)((big<<8) + little));
    gif.getFrame(p_frame)->setTranspColorIndex((unsigned int)m_fileContent[p_pointer++]);
    p_pointer++; //end-command "00"
}

void IO::setGCE(char *p_output, int &p_pointer, int p_frame)
{
    p_output[p_pointer++] = 2*16+1; //extension introducer (Hex:21)
    p_output[p_pointer++] = 15*16+9; //GCE labe (Hex:F9)
    p_output[p_pointer++] = 4; //blockSize
    char transColorFlag = gif.getFrame(p_frame)->getTranspColorFlag();
    p_output[p_pointer++] = transColorFlag; //PackedField
    p_output[p_pointer++] = 0; //delayTime, only in animated
    p_output[p_pointer++] = gif.getFrame(p_frame)->getTranspColorIndex(); //transparentColorIdenx
    p_output[p_pointer++] = 0; //Block terminator (always Hex: '00')
}

void IO::getIDiscr(int &p_pointer, int p_frame){
    int leftLittle = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    int leftBig = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.getFrame(p_frame)->setLeft((leftBig<<8)+leftLittle);

    unsigned int rightLittle = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    unsigned int rightBig = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.getFrame(p_frame)->setTop((rightBig<<8)+rightLittle);

    unsigned int widthLitte = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    unsigned int widthBig = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.getFrame(p_frame)->setWidth((widthBig<<8) + widthLitte);

    int heightLitte = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    int heightBig = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.getFrame(p_frame)->setHeight((heightBig<<8) + heightLitte);

    int packedByte = getNextByte(p_pointer);
    gif.getFrame(p_frame)->setLctFlag((unsigned int)getBit(packedByte, 7, 1));
    gif.getFrame(p_frame)->setInterlaceFlag((unsigned int)getBit(packedByte, 6, 1));
    gif.getFrame(p_frame)->setSortFlag((unsigned int)getBit(packedByte, 5, 1));
    gif.getFrame(p_frame)->setSizeOfLCT(zweiHochX(getBit(packedByte, 0, 3)+1));
}

void IO::setIDiscr(char *p_output, int &p_pointer, int p_frame)
{
    p_output[p_pointer++] = 2*16+12; //Image separator, always Hex: '2c'
    p_output[p_pointer++] = gif.getFrame(p_frame)->getLeft()%256; //image left (little endian)
    p_output[p_pointer++] = gif.getFrame(p_frame)->getLeft() - (gif.getFrame(p_frame)->getLeft()%256); //image left

    p_output[p_pointer++] = gif.getFrame(p_frame)->getTop()%256; //image top (little endian)
    p_output[p_pointer++] = gif.getFrame(p_frame)->getTop() - (gif.getFrame(p_frame)->getTop()%256); //image top

    p_output[p_pointer++] = gif.getFrame(p_frame)->getWidth()%256; //image width (little endian)
    p_output[p_pointer++] = gif.getFrame(p_frame)->getWidth() - (gif.getFrame(p_frame)->getWidth()%256); //image width

    p_output[p_pointer++] = gif.getFrame(p_frame)->getHeight()%256; //image height (little endian)
    p_output[p_pointer++] = gif.getFrame(p_frame)->getHeight() - (gif.getFrame(p_frame)->getHeight()%256); //image height

    char lctFlag = gif.getFrame(p_frame)->getLctFlag()?(1<<7):0;
    char sizeOfLCT = log2(gif.getFrame(p_frame)->getSizeOfLCT()-2)-1;
    p_output[p_pointer++] = lctFlag & sizeOfLCT; //packed Byte
}

void IO::getIData(int &p_pointer, int p_frame){
    gif.getFrame(p_frame)->setMinCodeSize(getBit(getNextByte(p_pointer), 0, 8));
    int next = getBit(getNextByte(p_pointer), 0, 8);
    int nextT = next;
    int k = 0;
    int pointerT = p_pointer;
    while(nextT != 0){
        k += nextT;
        pointerT += nextT;
        nextT = getBit(getNextByte(pointerT), 0, 8);
    }
    gif.getFrame(p_frame)->setSizeOfCodeTable(k);
    unsigned char* codeTable = new unsigned char[k];
    int j = 0;
    while(next > 0){
        int blockSize = next;
        for(int i = 0; i<blockSize; i++){ //Byte for Byte
            unsigned int byte = getBit(getNextByte(p_pointer), 0, 8);
            codeTable[j++] = getBinChar(byte);
        }
        next = getBit(getNextByte(p_pointer), 0, 8);
    }
    gif.getFrame(p_frame)->setCodeTable(codeTable, gif.getFrame(p_frame)->getSizeOfCodeTable());
    delete[] codeTable;
}

void IO::setIData(char *p_output, int &p_pointer, int p_frame)
{
    p_output[p_pointer++] = gif.getFrame(p_frame)->getMinCodeSize(); //min Code Size
    int bytes = gif.getFrame(p_frame)->getSizeOfCodeTable();
    for(int i = 0; i<bytes; ++i){
        if(i%256 == 0){
            p_output[p_pointer++] = bytes-i>256?256:bytes-i; //number of bytes in this block
        }
        p_output[p_pointer++] = gif.getFrame(p_frame)->getCodeTable()[i]; //codes
    }
    p_output[p_pointer++] = 0; //block terminator
}

void IO::setTrailer(char *p_output, int &p_pointer)
{
    p_output[p_pointer++] = 3*16+11;
}

void IO::getFile(char* p_fileName, char *p_content, int p_size)
{
    fstream file;
    file.open(p_fileName, ios::in);
    file.read(p_content, p_size);
    file.close();
}

void IO::saveFile(char* p_fileName, char *p_output, int p_fileSize)
{
    fstream file;
    file.open(p_fileName, ios::out);
    file.write(p_output, p_fileSize);
    file.close();
}

void IO::generateColorTable(Gif p_gif, int p_frame, vector<char> &p_codeTable)
{
    vector<char> color;
    int sizeOfColorTable = 0;
    for(int i = 0; i<p_gif.getFrame(p_frame)->getSizeOfPixel(); i+=3){
        int pos = isColorInTable(p_gif.getFrame(p_frame)->getPixel(), i, color);
        if(pos == 0){
            color.push_back(p_gif.getFrame(p_frame)->getPixel()[i]);
            color.push_back(p_gif.getFrame(p_frame)->getPixel()[i+1]);
            color.push_back(p_gif.getFrame(p_frame)->getPixel()[i+2]);
            p_codeTable.push_back((char)sizeOfColorTable++);
        } else {
            p_codeTable.push_back((char)pos);
        }
    }
    p_gif.getFrame(p_frame)->setLct(color, color.size());
    p_gif.getFrame(p_frame)->setSizeOfLCT(color.size());
}

int isColorInTable(char* pixel, int n, vector<char> color){
    for(int i = 0; i<color.size(); i+=3){
        if(color.at(i) == pixel[n]){
            for(int j = 1; j<color.size(); j+=3){
                if(color.at(j) == pixel[n+1]){
                    for(int k = 2; k<color.size(); k+=3){
                        if(color.at(k) == pixel[n+2]){
                            return i;
                        } else {
                            return 0;
                        }
                    }
                } else {
                    return 0;
                }
            }
        } else {
            return 0;
        }
    }
}



void IO::decompress(int img)
{
    int countPixel = gif.getFrame(img)->getWidth()*gif.getFrame(img)->getHeight()*3;
    if(gif.getFrame(img)->getLctFlag() == 1){
        //with LCT
        gif.getFrame(img)->setPixel(LZW::decode(gif.getFrame(img)->getCodeTable(), gif.getFrame(img)->getSizeOfCodeTable(), gif.getFrame(img)->getLct(), gif.getFrame(img)->getSizeOfLCT(), 1, countPixel), countPixel);
    } else {
        //with GCT
        gif.getFrame(img)->setPixel(LZW::decode(gif.getFrame(img)->getCodeTable(), gif.getFrame(img)->getSizeOfCodeTable(), gif.getColorTable(), gif.getSizeOfGCT(), 1, countPixel),countPixel);
    }
}

void IO::setHeader(char *p_output, int &p_pointer)
{
    p_output[p_pointer++] = 'G';
    p_output[p_pointer++] = 'I';
    p_output[p_pointer++] = 'F';
    p_output[p_pointer++] = '8';
    p_output[p_pointer++] = '9';
    p_output[p_pointer++] = 'a';
}

IO::IO(string p_filePath)
{
    m_fileName = p_filePath;
    m_gce = m_par = m_pte = m_appEx = m_commEx = 0;
    m_colorTable = NULL;
    m_fileContent = NULL;
}

IO::IO() {
    m_fileName = "";
    m_gce = m_par = m_pte = m_appEx = m_commEx = 0;
    m_colorTable = NULL;
    m_fileContent = NULL;
}

void IO::loadFile() {

    int fileSize = get_size((char*)m_fileName.c_str());
    m_fileContent = new char[fileSize];
    getFile((char*)m_fileName.c_str(), m_fileContent, fileSize);
    int pointer = 6;
    cout << endl;
    getScreen(pointer);
    if(gif.getGctFlag() == 1){
        getGCT(pointer);
    }
    m_gce = m_pte = m_appEx = m_commEx = 0;
    int next = getBit(getNextByte(pointer), 0, 8);
    int img = 0;
    while(next != 59){ //inner Loop until Trailer: 59 == '3B'
        if(next == 33){ //extension
            next = getNextByte(pointer);
            if(next == 255){
                //Application Extension
                cout << "app ext" << endl;
                continue;
            } else if(next == 254){
                //Comment Extension
                cout << "comm ext" << endl;
                continue;
            } else if(next == 249 || next == 1){ //GCE or PTE
                if(next == 249){ //Graphic Control Extension
                    if(img == gif.getSizeOfFrames()){
                        gif.extendImages(1);
                    }
                    cout << "gce" << endl;
                    m_gce = 1;
                    getGCE(pointer, img);
                }
                if(next == 1 || (getNextByte(pointer) == 33 && getNextByte(pointer) == 1)){
                    //Plain Text Extension
                    cout << "pte" << endl;
                    continue;
                }
            }
        }
        if(next == 44 || getNextByte(pointer) == 44){
            if(img == gif.getSizeOfFrames())
                gif.extendImages(1);
            getIDiscr(pointer, img);
            if(gif.getFrame(img)->getLctFlag() == 1)
                getLCT(pointer, img);

            getIData(pointer, img);
            img++;
        }
        next = getBit(getNextByte(pointer), 0, 8);
    }
    // LZW decompression
    for(int i = 0; i<gif.getSizeOfFrames(); ++i){
        decompress(i);
    }
//    gif.setPixel(gif.getImage(0)->getPixel());
}

void IO::generateFile()
{
    //first get the size of bytes to generate the file-char-array
    int fileSize = 13; //header+screenDescription
    if(gif.getGctFlag()){
        fileSize += gif.getSizeOfGCT(); //GCT
    }

    for(int i = 0; i<gif.getSizeOfFrames(); ++i){
        fileSize += 8; //GCE
        fileSize += 10; //ImageDiscriptor
        if(gif.getFrame(i)->getLctFlag()){
            fileSize += gif.getFrame(i)->getSizeOfLCT(); //LCT
        }
        int sizeOfCodeTable = 0;
        vector<char> codeTable; //sequenze of colors from LCT (uncompressed)
        generateColorTable(gif, i, codeTable); //generate LCT and set codeTable
        char* uncompCodeTable = new char[codeTable.size()];
        for(int i = 0; i<codeTable.size(); ++i){
            uncompCodeTable[i] = codeTable.at(i);
        }
        unsigned char *codes = LZW::encode(uncompCodeTable, codeTable.size(), gif.getFrame(i)->getLct(), gif.getFrame(i)->getSizeOfLCT(), sizeOfCodeTable);
        gif.getFrame(i)->setCodeTable(codes, sizeOfCodeTable);
        int blocks = sizeOfCodeTable/256 + (sizeOfCodeTable%256>0?1:0);
        fileSize += 1 + blocks + sizeOfCodeTable + 1; //minCodeSize + blockinfos + blockcontents + Block terminator

    }
    fileSize += 1; //Trailer
    char* output = new char[fileSize];
    int pointer = 0;
    setHeader(output, pointer);
    setScreen(output, pointer);
    if(gif.getGctFlag()){
        setGCT(output, pointer);
    }
    for(int i = 0; i<gif.getSizeOfFrames(); ++i){
        setGCE(output, pointer, i);
        setIDiscr(output, pointer, i);
        if(gif.getFrame(i)->getLctFlag()){
            setLCT(output, pointer, i);
        }
        setIData(output, pointer, i);
    }
    setTrailer(output, pointer);
    saveFile((char*)m_fileName.c_str(), output, fileSize);
}

Gif *IO::getGif()
{
    return &gif;
}

int IO::getBit(int p_wert, int p_bit, int p_count){
   return (p_wert & ((zweiHochX(p_count)-1) << p_bit))>>p_bit;
}

int IO::zweiHochX(int p_exponent){
    int res = 1;
    for(int i = 0; i<p_exponent; i++)
        res *= 2;
    return res;
}

char IO::getHex(int p_binary){
    if(p_binary>9)
        return static_cast<char>(p_binary+55);
    else
        return static_cast<char>(p_binary+48);
}

unsigned char IO::getBinChar(unsigned int p_binary){
    return (unsigned char)getBit(p_binary, 0, 8);
}
unsigned int IO::getNextByte(int &p_pointer){
    return (unsigned int)m_fileContent[p_pointer++];
}

int get_size(const char* s){
   FILE* file = fopen(s, "rb");
   fseek(file, 0L, SEEK_END);
   int size = ftell(file);
   fclose(file);
   return size;
}
