#include "gif.h"
#include <fstream>
#include <iostream>

using namespace std;

int getBit(int wert, int bit, int anz);
int zweiHochX(int x);
char getHex(int i);
unsigned char getBinChar(unsigned int b);
int get_size(const char *file);
void Gif::getScreen(int& pointer){
    //width of image
    int little = 255 & (unsigned int)(cstring[pointer++]);
    int big = 255 & (unsigned int)(cstring[pointer++]);
    width = (big<<8) + little;

    //height of image
    int big2 = 255 & (unsigned int)(cstring[pointer+1]);
    int little2 = 255 & (unsigned int)(cstring[pointer++]);
    height = (big2<<8) + little2;

    //packed Bytea
    int packedByte = (unsigned int)(cstring[(++pointer)++]);
    gctf = getBit(packedByte, 7, 1);
    colorRes = getBit(packedByte, 4, 3);
    sortFlag = getBit(packedByte, 3, 1);
    sizeOfColorTable = zweiHochX(1+getBit(packedByte, 0, 3));

    //Background Color Image
    bgColor = getBit((unsigned int)(cstring[pointer++]), 0, 8);

    //Pixel Aspect Ratio
    par = getBit((unsigned int)(cstring[pointer++]),0,8);
}

void Gif::getCT(int count, int &pointer, char* &table){
    table = new char[count*6];
    for(int i = 0; i<count; ++i){
        int colorR = (unsigned int)(cstring[pointer++]);
        table[i*6] = getHex(getBit(colorR, 4, 4));
        table[i*6+1] = getHex(getBit(colorR, 0, 4));
        int colorG = (unsigned int)(cstring[pointer++]);
        table[i*6+2] = getHex(getBit(colorG, 4, 4));
        table[i*6+3] = getHex(getBit(colorG, 0, 4));
        int colorB = (unsigned int)(cstring[pointer++]);
        table[i*6+4] = getHex(getBit(colorB, 4, 4));
        table[i*6+5] = getHex(getBit(colorB, 0, 4));
        cout << "color" << i << ": " << table[i*6] <<  table[i*6+1] << table[i*6+2] << table[i*6+3] << table[i*6+4] << table[i*6+5] << endl;
    }
}

void Gif::getGCE(int &pointer){ //Graphics Control Extension
    int blockSize = (unsigned int)cstring[pointer++];
    int packed = (unsigned int)cstring[pointer++];
    int disposalM = getBit(packed, 2, 3);
    int userInputFlag = getBit(packed, 1, 1);
    int transpColorFlag = getBit(packed, 0, 1);
    int little = 255 & ((unsigned int)(cstring[pointer++]));
    int big = 255 & (unsigned int)(cstring[pointer++]);
    delayTime = (big<<8) + little;
    int transpColorIndex = (unsigned int)cstring[pointer++];
    pointer++; //end-command "00"
}

void Gif::getIDiscr(int &pointer){
    int leftLittle = 255 & (unsigned int)(cstring[pointer++]);
    int leftBig = 255 & (unsigned int)(cstring[pointer++]);
    int left = (leftBig<<8)+leftLittle;
    cout << "left: " << left << endl;


    int rightLittle = 255 & (unsigned int)(cstring[pointer++]);
    int rightBig = 255 & (unsigned int)(cstring[pointer++]);
    int right = (rightBig<<8)+rightLittle;
    cout << "right: " << right << endl;

    int widthLitte = 255 && (unsigned int)(cstring[pointer++]);
    int widthBig = 255 & (unsigned int)(cstring[pointer++]);
    int width = (widthBig<<8) + widthLitte;
    cout << "width: " << width << endl;

    int heightLitte = 255 && (unsigned int)(cstring[pointer++]);
    int heightBig = 255 & (unsigned int)(cstring[pointer++]);
    int height = (heightBig<<8) + heightLitte;
    cout << "height: " << height << endl;

    int packedByte = getNext(pointer);
    lctFlag = getBit(packedByte, 7, 1);
    cout << "LCT-Flag: " << lctFlag << endl;
    int interlaceFlag = getBit(packedByte, 6, 1);
    int sortFlag = getBit(packedByte, 5, 1);
    localColorTable = getBit(packedByte, 0, 3);
    cout << "LCT-Size: " << localColorTable << endl;
}

void Gif::getIData(int &pointer){
    cout << "pointer: --------" << pointer << endl;
    unsigned int minCodeSize = getBit(getNext(pointer), 0, 8);
    cout << "min Code Size: " << minCodeSize << endl;
    int next = getBit(getNext(pointer), 0, 8);
    int nextT = next;
    int k = 0;
    int pointerT = pointer;
    while(nextT != 0){
        k += nextT;
        pointerT += nextT;
        nextT = getBit(getNext(pointerT), 0, 8);
    }
    sizeOfCodeTable = k;
    codeTable = new unsigned char[sizeOfCodeTable];
    int j = 0;
    while(next > 0){
        int blockSize = next;
        for(int i = 0; i<blockSize; i++){ //Byte for Byte
            unsigned int byte = getBit(getNext(pointer), 0, 8);
            codeTable[j++] = getBinChar(byte);
        }
        next = getBit(getNext(pointer), 0, 8);
    }
}

Gif::Gif(char* s)
{
    fstream file;
    file.open(s, ios::in);
    int fileSize = get_size(s);
    cout << "filesize: " << fileSize << endl;
    cstring = new char[fileSize];
    file.read(cstring, fileSize);
    for(int i = 0; i<6; i++)
    {
        cout << cstring[i];
    }
    int pointer = 6;
    cout << endl;
    getScreen(pointer);
    cout << "breite: " << width << endl;
    cout << "hoehe: " << height << endl;
    cout << "Global Color Table Flag: " << gctf << endl;
    cout << "Color Resolution: " << colorRes << endl;
    cout << "Sort Flag to Global Color Table: " << sortFlag << endl;
    cout << "Size of Global Color Table: " << sizeOfColorTable << endl;
    cout << "Background Color Index: " << bgColor << endl;
    cout << "Pixel Aspect Ratio: " << par << endl;
    if(gctf == 1){
        getCT(sizeOfColorTable, pointer, colorTable);
    }
    gce = pte = appEx = commEx = 0;
    int next = getBit(getNext(pointer), 0, 8);
    cout << "next: " << next << endl;
    while(next != 59){ //inner Loop until Trailer: 59 == '3B'
        cout << next << "ext? ";
        if(next == 33){ //extension
            cout << "ext" << endl;
            next = getNext(pointer);
            if(next == 255){
                //Application Extension
                cout << "app ext" << endl;
                break;
            } else if(next == 254){
                //Comment Extension
                cout << "comm ext" << endl;
                break;
            } else if(next == 249 || next == 1){ //GCE or PTE
                if(next == 249){ //Graphic Control Extension
                    cout << "gce" << endl;
                    gce = 1;
                    getGCE(pointer);
                }
                if(next == 1 || (getNext(pointer) == 33 && getNext(pointer) == 1)){
                    //Plain Text Extension
                    cout << "pte" << endl;
                    break;
                }
            }
        }
        if(next == 44 || getNext(pointer) == 44){
            getIDiscr(pointer);
            if(lctFlag == 1)
                getCT(localColorTable, pointer, colorTable);
            getIData(pointer);

        }
        next = getBit(getNext(pointer), 0, 8);
    }
    cout << "Trailer: ";
    cout << (getHex(getBit((unsigned int)cstring[pointer-1],4,4))) << getHex(getBit((unsigned int)cstring[pointer-1],0,4)) << endl;
    file.close();

    for(int i = 0; i<sizeOfCodeTable; i++){
//        cout << getBit(codeTable[i], 0, 8);
        for(int j = 0; j<8; j++)
            cout << getBit(codeTable[i], j, 1);
        cout << " ";
    }
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
unsigned int Gif::getNext(int &pointer){
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
