#include "io.h"
#include "lzw.h"
#include <fstream>
#include <iostream>

using namespace std;

int getBit(int wert, int bit, int anz);

int zweiHochX(int x);

char getHex(int i);

unsigned char getBinChar(unsigned int b);

int get_size(const char *file);

void IO::setScreen(int& pointer){
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
    colorRes = getBit(packedByte, 4, 3);
    sortFlag = getBit(packedByte, 3, 1);
    gif.setSizeOfGCT(zweiHochX(1+getBit(packedByte, 0, 3)));

    //Background Color Image
    gif.setBgColor(getBit((unsigned int)(cstring[pointer++]), 0, 8));

    //Pixel Aspect Ratio
    par = getBit((unsigned int)(cstring[pointer++]),0,8);
}

void IO::getLCT(int &pointer, int img){
    char* table = new char[gif.getImage(img).getSizeOfLCT()*6];
    for(int i = 0; i<gif.getImage(img).getSizeOfLCT(); ++i){
        int colorR = (unsigned int)(cstring[pointer++]);
        table[i*6] = getHex(getBit(colorR, 4, 4));
        table[i*6+1] = getHex(getBit(colorR, 0, 4));
        int colorG = (unsigned int)(cstring[pointer++]);
        table[i*6+2] = getHex(getBit(colorG, 4, 4));
        table[i*6+3] = getHex(getBit(colorG, 0, 4));
        int colorB = (unsigned int)(cstring[pointer++]);
        table[i*6+4] = getHex(getBit(colorB, 4, 4));
        table[i*6+5] = getHex(getBit(colorB, 0, 4));
        gif.getImage(img).setLct(table);
        cout << "color" << i << ": " << table[i*6] <<  table[i*6+1] << table[i*6+2] << table[i*6+3] << table[i*6+4] << table[i*6+5] << endl;
    }
}

void IO::getGCT(int &pointer){
    char* table = new char[gif.getSizeOfGCT()*6];
    for(int i = 0; i<gif.getSizeOfGCT(); ++i){
        int colorR = (unsigned int)(cstring[pointer++]);
        table[i*6] = getHex(getBit(colorR, 4, 4));
        table[i*6+1] = getHex(getBit(colorR, 0, 4));
        int colorG = (unsigned int)(cstring[pointer++]);
        table[i*6+2] = getHex(getBit(colorG, 4, 4));
        table[i*6+3] = getHex(getBit(colorG, 0, 4));
        int colorB = (unsigned int)(cstring[pointer++]);
        table[i*6+4] = getHex(getBit(colorB, 4, 4));
        table[i*6+5] = getHex(getBit(colorB, 0, 4));
        gif.setColorTable(table);
        cout << "color" << i << ": " << table[i*6] <<  table[i*6+1] << table[i*6+2] << table[i*6+3] << table[i*6+4] << table[i*6+5] << endl;
    }
}

void IO::getGCE(int &pointer, int img){ //Graphics Control Extension
    int blockSize = (unsigned int)cstring[pointer++];
    int packed = (unsigned int)cstring[pointer++];
    int disposalM = getBit(packed, 2, 3);
    int userInputFlag = getBit(packed, 1, 1);
    gif.getImage(img).setTranspColorFlag(getBit(packed, 0, 1));
    int little = 255 & ((unsigned int)(cstring[pointer++]));
    int big = 255 & (unsigned int)(cstring[pointer++]);
    gif.getImage(img).setDelayTime((big<<8) + little);
    gif.getImage(img).setTranspColorIndex((unsigned int)cstring[pointer++]);
    pointer++; //end-command "00"
}

void IO::getIDiscr(int &pointer, int img){
    int leftLittle = 255 & (unsigned int)(cstring[pointer++]);
    int leftBig = 255 & (unsigned int)(cstring[pointer++]);
    gif.getImage(img).setLeft((leftBig<<8)+leftLittle);


    int rightLittle = 255 & (unsigned int)(cstring[pointer++]);
    int rightBig = 255 & (unsigned int)(cstring[pointer++]);
    gif.getImage(img).setHeight((rightBig<<8)+rightLittle);

    int widthLitte = 255 && (unsigned int)(cstring[pointer++]);
    int widthBig = 255 & (unsigned int)(cstring[pointer++]);
    gif.getImage(img).setWidth((widthBig<<8) + widthLitte);

    int heightLitte = 255 && (unsigned int)(cstring[pointer++]);
    int heightBig = 255 & (unsigned int)(cstring[pointer++]);
    gif.getImage(img).setHeight((heightBig<<8) + heightLitte);

    int packedByte = getNextByte(pointer);
    gif.getImage(img).setLctFlag(getBit(packedByte, 7, 1));
    int interlaceFlag = getBit(packedByte, 6, 1); //unused
    gif.getImage(img).setSortFlag(getBit(packedByte, 5, 1));
    gif.getImage(img).setSizeOfLCT(zweiHochX(getBit(packedByte, 0, 3)+1));
}

void IO::getIData(int &pointer, int img){
    cout << "pointer: --------" << pointer << endl;
    gif.getImage(img).setMinCodeSize(getBit(getNextByte(pointer), 0, 8));
    int next = getBit(getNextByte(pointer), 0, 8);
    int nextT = next;
    int k = 0;
    int pointerT = pointer;
    while(nextT != 0){
        k += nextT;
        pointerT += nextT;
        nextT = getBit(getNextByte(pointerT), 0, 8);
    }
    gif.getImage(img).setSizeOfLCT(k);
    unsigned char* codeTable = new unsigned char[gif.getImage(img).getSizeOfLCT()];
    int j = 0;
    while(next > 0){
        int blockSize = next;
        for(int i = 0; i<blockSize; i++){ //Byte for Byte
            unsigned int byte = getBit(getNextByte(pointer), 0, 8);
            codeTable[j++] = getBinChar(byte);
        }
        next = getBit(getNextByte(pointer), 0, 8);
    }
    gif.getImage(img).setCodeTable(codeTable);
}

void IO::getFile(char *s, int n)
{
    fstream file;
    file.open(s, ios::in);
    file.read(s, n);
    file.close();
}

void IO::decompress(int img)
{
    if(gif.getImage(img).getLctFlag() == 1){
        //with LCT
        gif.getImage(img).setPixles(LZW::decode(gif.getImage(img).getCodeTable(), gif.getImage(img).getSizeOfCodeTable(), gif.getImage(img).getLct(), gif.getImage(img).getSizeOfLCT(), 1));
    } else {
        //with GCT
        gif.getImage(img).setPixles(LZW::decode(gif.getImage(img).getCodeTable(), gif.getImage(img).getSizeOfCodeTable(), gif.getColorTable(), gif.getSizeOfGCT(), 1));
    }
}

IO::IO(char* s)
{
    int fileSize = get_size(s);
    cstring = new char[fileSize];
    getFile(cstring, fileSize);
    int pointer = 6;
    cout << endl;
    setScreen(pointer);
    if(gctf == 1){
        getGCT(pointer);
    }
    gce = pte = appEx = commEx = 0;
    int next = getBit(getNextByte(pointer), 0, 8);
    int img = 0;
    Image* images = new Image[1];
    gif.setImages(images);
    gif.setSizeOfImages(1);
    while(next != 59){ //inner Loop until Trailer: 59 == '3B'
        if(next == 33){ //extension
            cout << "ext" << endl;
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
                    if(img = gif.getSizeOfImages()){
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
            if(img = gif.getSizeOfImages())
                gif.extendImages(1);
            getIDiscr(pointer, img);
            if(gif.getImage(img).getLctFlag() == 1)
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
}

Gif IO::getGif()
{
    return gif;
}

int getBit(int wert, int bit, int anz){
   return (wert & ((zweiHochX(anz)-1) << bit))>>bit;
}

int zweiHochX(int x){
    int res = 1;
    for(int i = 0; i<x; i++)
        res *= 2;
    return res;
}

char getHex(int i){
    if(i>9)
        return static_cast<char>(i+55);
    else
        return static_cast<char>(i+48);
}
unsigned int IO::getNextByte(int &pointer){
    return (unsigned int)cstring[pointer++];
}

unsigned char getBinChar(unsigned int b){
    return (unsigned char)getBit(b, 0, 8);
}

int get_size(const char* s){
   FILE* file = fopen(s, "rb");
   fseek(file, 0L, SEEK_END);
   int size = ftell(file);
   fclose(file);
   return size;
}
