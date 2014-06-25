#include "io.h"
#include "lzw.h"
#include "gif.h"
#include <math.h>
#include <fstream>
#include <iostream>

using namespace std;



int get_size(const char *file);

void IO::getScreen(int& pointer){
    //width of image
    int little = 255 & (unsigned int)(cstring[pointer++]);
    int big = 255 & (unsigned int)(cstring[pointer++]);
    gif.setWidth((big<<8) + little);

    //height of image
    int big2 = 255 & (unsigned int)(cstring[pointer+1]);
    int little2 = 255 & (unsigned int)(cstring[pointer++]);
    gif.setHeight((big2<<8) + little2);

    //packed Bytea
    int packedByte = (unsigned int)(cstring[(++pointer)++]);
    gif.setGctFlag(getBit(packedByte, 7, 1));
    gif.setSizeOfGCT(zweiHochX(1+getBit(packedByte, 0, 3)));

    //Background Color Image
    gif.setBgColor(getBit((unsigned int)(cstring[pointer++]), 0, 8));

    //Pixel Aspect Ratio
    par = getBit((unsigned int)(cstring[pointer++]),0,8);
}

void IO::setScreen(char *output, int &pointer)
{
    output[pointer++] = gif.getWidth()%256; //gif width (little endian)
    output[pointer++] = gif.getWidth() - (gif.getWidth()%256); //gif width
    output[pointer++] = gif.getHeight()%256; //gif height (little endian)
    output[pointer++] = gif.getHeight() - (gif.getHeight()%256); //gif height
    char gctFlag = 0;
    if(gif.getGctFlag())
        gctFlag = 1<<7;
    char colorRes = 1<<4;
    char sizeOfGCT = log2(gif.getSizeOfGCT()-2)-1;
    output[pointer++] = gctFlag & colorRes & sizeOfGCT; //packed Byte
    output[pointer++] = gif.getBgColor(); //Background color index
    output[pointer++] = 0; //Pixel Aspect Ratio
}

void IO::getLCT(int &pointer, int img){
    char* table = new char[gif.getImage(img)->getSizeOfLCT()*3];
    for(int i = 0; i<gif.getImage(img)->getSizeOfLCT(); ++i){
        table[i*3] = cstring[pointer++];
        table[i*3+1] = cstring[pointer++];
        table[i*3+2] = cstring[pointer++];
        gif.getImage(img)->setLct(table, gif.getImage(img)->getSizeOfLCT());
        cout << "color" << i << ": " << (unsigned int)table[i*3] <<  (unsigned int)table[i*3+1] << (unsigned int)table[i*3+2] << endl;
        delete[] table;
    }
}

void IO::setLCT(char *output, int &pointer, int img)
{
    for(int i = 0; i<gif.getImage(img)->getSizeOfLCT()-2; ++i){ //last 2 codes not in file!
        output[pointer++] = gif.getImage(img)->getLct()[i]; //lines of Colors
    }
}

void IO::getGCT(int &pointer){
    cout << "colorTable" << endl;
    char* table = new char[gif.getSizeOfGCT()*3];
    for(int i = 0; i<gif.getSizeOfGCT(); ++i){
        table[i*3] = cstring[pointer++];
        table[i*3+1] = cstring[pointer++];
        table[i*3+2] = cstring[pointer++];
        gif.setColorTable(table, gif.getSizeOfGCT());
        cout << "color" << i << ": " << (unsigned int)table[i*3] <<  (unsigned int)table[i*3+1] << (unsigned int)table[i*3+2] << endl;
    }
}

void IO::setGCT(char *output, int &pointer)
{
    for(int i = 0; i<gif.getSizeOfGCT()-2; ++i){ //last 2 codes not in file!
        output[pointer++] = gif.getColorTable()[i]; //lines of Colors
    }
}

void IO::getGCE(int &pointer, int img){ //Graphics Control Extension
    int blockSize = (unsigned int)cstring[pointer++];
    int packed = (unsigned int)cstring[pointer++];
    int disposalM = getBit(packed, 2, 3);
    int userInputFlag = getBit(packed, 1, 1);
    gif.getImage(img)->setTranspColorFlag(getBit(packed, 0, 1));
    int little = 255 & ((unsigned int)(cstring[pointer++]));
    int big = 255 & (unsigned int)(cstring[pointer++]);
    gif.getImage(img)->setDelayTime((big<<8) + little);
    gif.getImage(img)->setTranspColorIndex((unsigned int)cstring[pointer++]);
    pointer++; //end-command "00"
}

void IO::setGCE(char *output, int &pointer, int img)
{
    output[pointer++] = 2*16+1; //extension introducer (Hex:21)
    output[pointer++] = 15*16+9; //GCE labe (Hex:F9)
    output[pointer++] = 4; //blockSize
    char transColorFlag = gif.getImage(img)->getTranspColorFlag();
    output[pointer++] = transColorFlag; //PackedField
    output[pointer++] = 0; //delayTime, only in animated
    output[pointer++] = gif.getImage(img)->getTranspColorIndex(); //transparentColorIdenx
    output[pointer++] = 0; //Block terminator (always Hex: '00')
}

void IO::getIDiscr(int &pointer, int img){
    int leftLittle = 255 & (unsigned int)(cstring[pointer++]);
    int leftBig = 255 & (unsigned int)(cstring[pointer++]);
    gif.getImage(img)->setLeft((leftBig<<8)+leftLittle);

    unsigned int rightLittle = 255 & (unsigned int)(cstring[pointer++]);
    unsigned int rightBig = 255 & (unsigned int)(cstring[pointer++]);
    gif.getImage(img)->setTop((rightBig<<8)+rightLittle);

    unsigned int widthLitte = 255 & (unsigned int)(cstring[pointer++]);
    unsigned int widthBig = 255 & (unsigned int)(cstring[pointer++]);
    gif.getImage(img)->setWidth((widthBig<<8) + widthLitte);

    int heightLitte = 255 & (unsigned int)(cstring[pointer++]);
    int heightBig = 255 & (unsigned int)(cstring[pointer++]);
    gif.getImage(img)->setHeight((heightBig<<8) + heightLitte);

    int packedByte = getNextByte(pointer);
    gif.getImage(img)->setLctFlag(getBit(packedByte, 7, 1));
    int interlaceFlag = getBit(packedByte, 6, 1); //unused
    gif.getImage(img)->setSortFlag(getBit(packedByte, 5, 1));
    gif.getImage(img)->setSizeOfLCT(zweiHochX(getBit(packedByte, 0, 3)+1));
}

void IO::setIDiscr(char *output, int &pointer, int img)
{
    output[pointer++] = 2*16+12; //Image separator, always Hex: '2c'
    output[pointer++] = gif.getImage(img)->getLeft()%256; //image left (little endian)
    output[pointer++] = gif.getImage(img)->getLeft() - (gif.getImage(img)->getLeft()%256); //image left

    output[pointer++] = gif.getImage(img)->getTop()%256; //image top (little endian)
    output[pointer++] = gif.getImage(img)->getTop() - (gif.getImage(img)->getTop()%256); //image top

    output[pointer++] = gif.getImage(img)->getWidth()%256; //image width (little endian)
    output[pointer++] = gif.getImage(img)->getWidth() - (gif.getImage(img)->getWidth()%256); //image width

    output[pointer++] = gif.getImage(img)->getHeight()%256; //image height (little endian)
    output[pointer++] = gif.getImage(img)->getHeight() - (gif.getImage(img)->getHeight()%256); //image height

    char lctFlag = gif.getImage(img)->getLctFlag()?(1<<7):0;
    char sizeOfLCT = log2(gif.getImage(img)->getSizeOfLCT()-2)-1;
    output[pointer++] = lctFlag & sizeOfLCT; //packed Byte
}

void IO::getIData(int &pointer, int img){
    cout << "pointer: --------" << pointer << endl;
    gif.getImage(img)->setMinCodeSize(getBit(getNextByte(pointer), 0, 8));
    int next = getBit(getNextByte(pointer), 0, 8);
    int nextT = next;
    int k = 0;
    int pointerT = pointer;
    while(nextT != 0){
        k += nextT;
        pointerT += nextT;
        nextT = getBit(getNextByte(pointerT), 0, 8);
    }
    cout << "k: " << k << endl;
    gif.getImage(img)->setSizeOfCodeTable(k);
    cout << "width: " << gif.getImage(img)->getWidth() << endl;
    unsigned char* codeTable = new unsigned char[k];
    int j = 0;
    while(next > 0){
        int blockSize = next;
        for(int i = 0; i<blockSize; i++){ //Byte for Byte
            unsigned int byte = getBit(getNextByte(pointer), 0, 8);
            codeTable[j++] = getBinChar(byte);
        }
        next = getBit(getNextByte(pointer), 0, 8);
    }
    cout << "img: " << img << endl;
    cout << "size of codetable: " << gif.getImage(img)->getSizeOfCodeTable() << endl;
    gif.getImage(img)->setCodeTable(codeTable, gif.getImage(img)->getSizeOfCodeTable());
//    for(int i = 0; i<gif.getImage(img)->getSizeOfCodeTable(); ++i){
//        cout << i << ": ";
//        for(int j = 0; j<8; ++j){
//            cout << getBit(codeTable[i], j, 1);
//        }
//        cout << endl;
//        cout << i << ": ";
//        for(int j = 0; j<8; ++j){
//            cout << getBit(gif.getImage(img)->getCodeTable()[i], j, 1);
//        }
//        cout << endl;
    //    }
}

void IO::setIData(char *output, int &pointer, int img)
{
    output[pointer++] = gif.getImage(img)->getMinCodeSize(); //min Code Size
    int bytes = gif.getImage(img)->getSizeOfCodeTable();
    for(int i = 0; i<bytes; ++i){
        if(i%256 == 0){
            output[pointer++] = bytes-i>256?256:bytes-i; //number of bytes in this block
        }
        output[pointer++] = gif.getImage(img)->getCodeTable()[i]; //codes
    }
    output[pointer++] = 0; //block terminator
}

void IO::setTrailer(char *output, int &pointer)
{
    output[pointer++] = 3*16+11;
}

void IO::getFile(char* fileName, char *s, int n)
{
    fstream file;
    file.open(fileName, ios::in);
    file.read(s, n);
    file.close();
}

void IO::saveFile(char* fileName, char *output, int fileSize)
{
    fstream file;
    file.open(fileName, ios::out);
    file.write(output, fileSize);
    file.close();
}

IO::IO()
{

}

void IO::decompress(int img)
{
    int countPixel = gif.getImage(img)->getWidth()*gif.getImage(img)->getHeight()*3;
    if(gif.getImage(img)->getLctFlag() == 1){
        //with LCT
        gif.getImage(img)->setPixel(LZW::decode(gif.getImage(img)->getCodeTable(), gif.getImage(img)->getSizeOfCodeTable(), gif.getImage(img)->getLct(), gif.getImage(img)->getSizeOfLCT(), 1, countPixel), countPixel);
    } else {
        //with GCT
        for(int i = 0; i<gif.getSizeOfGCT(); ++i){
            cout << (char)gif.getColorTable()[i] << endl;
        }
        gif.getImage(img)->setPixel(LZW::decode(gif.getImage(img)->getCodeTable(), gif.getImage(img)->getSizeOfCodeTable(), gif.getColorTable(), gif.getSizeOfGCT(), 1, countPixel),countPixel);
    }
}

void IO::setHeader(char *output, int &pointer)
{
    output[pointer++] = 'G';
    output[pointer++] = 'I';
    output[pointer++] = 'F';
    output[pointer++] = '8';
    output[pointer++] = '9';
    output[pointer++] = 'a';
}

IO::IO(char* s)
{
    fileName = s;
}

void IO::loadFile() {

    int fileSize = get_size(fileName);
    cstring = new char[fileSize];
    getFile(fileName, cstring, fileSize);
    int pointer = 6;
    cout << endl;
    getScreen(pointer);
    if(gif.getGctFlag() == 1){
        getGCT(pointer);
    }
    gce = pte = appEx = commEx = 0;
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
                    if(img == gif.getSizeOfImages()){
                        gif.extendImages(1);
                    }
                    cout << "gce" << endl;
                    gce = 1;
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
            if(img == gif.getSizeOfImages())
                gif.extendImages(1);
            getIDiscr(pointer, img);
            if(gif.getImage(img)->getLctFlag() == 1)
                getLCT(pointer, img);

            getIData(pointer, img);
            img++;
        }
        next = getBit(getNextByte(pointer), 0, 8);
    }
    // LZW decompression
    for(int i = 0; i<gif.getSizeOfImages(); ++i){
        decompress(i);
    }
    gif.setPixel(gif.getImage(0)->getPixel());
}

void IO::generateFile()
{
    int fileSize = 13; //header+screenDescription
    if(gif.getGctFlag()){
        fileSize += gif.getSizeOfGCT(); //GCT
    }
    for(int i = 0; i<gif.getSizeOfImages(); ++i){
        fileSize += 8; //GCE
        fileSize += 10; //ImageDiscriptor
        if(gif.getImage(i)->getLctFlag()){
            fileSize += gif.getImage(i)->getSizeOfLCT(); //LCT
        }
        int sizeOfCodeTable = 0;
        unsigned char *codes = LZW::encode(gif.getImage(i)->getPixel(), gif.getImage(i)->getSizeOfPixel(), sizeOfCodeTable);
        gif.getImage(i)->setCodeTable(codes, sizeOfCodeTable);
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
    for(int i = 0; i<gif.getSizeOfImages(); ++i){
        setGCE(output, pointer, i);
        setIDiscr(output, pointer, i);
        if(gif.getImage(i)->getLctFlag()){
            setLCT(output, pointer, i);
        }
        setIData(output, pointer, i);
    }
    setTrailer(output, pointer);
    saveFile(fileName, output, fileSize);
}

Gif *IO::getGif()
{
    return &gif;
}

int IO::getBit(int wert, int bit, int anz){
   return (wert & ((zweiHochX(anz)-1) << bit))>>bit;
}

int IO::zweiHochX(int x){
    int res = 1;
    for(int i = 0; i<x; i++)
        res *= 2;
    return res;
}

char IO::getHex(int i){
    if(i>9)
        return static_cast<char>(i+55);
    else
        return static_cast<char>(i+48);
}

unsigned char IO::getBinChar(unsigned int b){
    return (unsigned char)getBit(b, 0, 8);
}
unsigned int IO::getNextByte(int &pointer){
    return (unsigned int)cstring[pointer++];
}

int get_size(const char* s){
   FILE* file = fopen(s, "rb");
   fseek(file, 0L, SEEK_END);
   int size = ftell(file);
   fclose(file);
   return size;
}
