#include "io.h"
#include "lzw.h"
#include "gif.h"
#include "interlacedpicture.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;



/**
 * @brief
 *
 * @param file
 * @return int
 */
int get_size(const char *file);
/**
 * @brief
 *
 * @param pixel
 * @param n
 * @param color
 * @return int
 */
int isColorInTable(char* pixel, int n, vector<char> color);

/**
 * @brief
 *
 * @param p_pointer
 */
void IO::getScreen(int& p_pointer){
    //width of image
    int little = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    int big = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.setWidth((big<<8) + little);

    //height of image
    int big2 = 255 & (unsigned int)(m_fileContent[p_pointer+1]);
    int little2 = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.setHeight((big2<<8) + little2);

    //packed Byte
    int packedByte = (unsigned int)(m_fileContent[(++p_pointer)++]);
    gif.setGctFlag(getBit(packedByte, 7, 1));
    if(gif.getGctFlag() == 1){
        gif.setSizeOfGCT(zweiHochX(1+getBit(packedByte, 0, 3)));
    }

    //Background Color Image
    gif.setBgColor(getBit((unsigned int)(m_fileContent[p_pointer++]), 0, 8));

    //Pixel Aspect Ratio
    m_par = getBit((unsigned int)(m_fileContent[p_pointer++]),0,8);
}

/**
 * @brief
 *
 * @param p_output
 * @param p_pointer
 */
void IO::setScreen(char *p_output, int &p_pointer)
{
    p_output[p_pointer++] = gif.getWidth()%256; //gif width (little endian)
    p_output[p_pointer++] = gif.getWidth() - (gif.getWidth()%256); //gif width
    p_output[p_pointer++] = gif.getHeight()%256; //gif height (little endian)
    p_output[p_pointer++] = gif.getHeight() - (gif.getHeight()%256); //gif height
    int gctFlag = 0;
    if(gif.getGctFlag() == 1){
        gctFlag = 1<<7;
    }
    int colorRes = 1<<4;
    int sizeOfGCT = log2(gif.getSizeOfGCT())-1;
    unsigned char packed = (unsigned char)(gctFlag | colorRes | sizeOfGCT);
    p_output[p_pointer++] = packed; //packed Byte
    p_output[p_pointer++] = 0; //Background color index
    p_output[p_pointer++] = 0; //Pixel Aspect Ratio
}

/**
 * @brief
 *
 * @param p_pointer
 * @param p_frame
 */
void IO::getLCT(int &p_pointer, int p_frame){
//    cout << "LCT " << p_frame << endl;
    m_lctTable = new char[gif.getFrame(p_frame)->getSizeOfLCT()*3];
    for(int i = 0; i<gif.getFrame(p_frame)->getSizeOfLCT(); ++i){
        m_lctTable[i*3] = m_fileContent[p_pointer++];
        m_lctTable[i*3+1] = m_fileContent[p_pointer++];
        m_lctTable[i*3+2] = m_fileContent[p_pointer++];
    }
    gif.getFrame(p_frame)->setLct(m_lctTable, gif.getFrame(p_frame)->getSizeOfLCT());
}

/**
 * @brief
 *
 * @param p_output
 * @param p_pointer
 * @param p_frame
 */
void IO::setLCT(char *p_output, int &p_pointer, int p_frame)
{
    for(int i = 0; i<gif.getFrame(p_frame)->getSizeOfLCT()-2; ++i){ //last 2 codes not in file!
        p_output[p_pointer++] = gif.getFrame(p_frame)->getLct()[i]; //lines of Colors
    }
}

/**
 * @brief
 *
 * @param p_pointer
 */
void IO::getGCT(int &p_pointer){
    m_gctTable = new char[gif.getSizeOfGCT()*3];
    for(int i = 0; i<gif.getSizeOfGCT(); ++i){
        m_gctTable[i*3] = m_fileContent[p_pointer++];
        m_gctTable[i*3+1] = m_fileContent[p_pointer++];
        m_gctTable[i*3+2] = m_fileContent[p_pointer++];
    }
    gif.setColorTable(m_gctTable, gif.getSizeOfGCT());
}

/**
 * @brief
 *
 * @param p_output
 * @param p_pointer
 */
void IO::setGCT(char *p_output, int &p_pointer)
{
    for(int i = 0; i<gif.getSizeOfGCT()*3; ++i){ //last 2 codes not in file!
        p_output[p_pointer++] = gif.getColorTable()[i]; //lines of Colors
    }
}

/**
 * @brief
 *
 * @param p_pointer
 * @param p_frame
 */
void IO::getGCE(int &p_pointer, int p_frame){ //Graphics Control Extension
    p_pointer++; //blocksize, always 4!
    int packed = (unsigned int)m_fileContent[p_pointer++]; //packed Byte, the following three variables are the value
    gif.getFrame(p_frame)->setDisposualMethod((unsigned int)getBit(packed, 2, 3));
    gif.getFrame(p_frame)->setUserInputFlag((unsigned int)getBit(packed, 1, 1));
    gif.getFrame(p_frame)->setTranspColorFlag((unsigned int)getBit(packed, 0, 1));
    int little = 255 & ((unsigned int)(m_fileContent[p_pointer++]));
    int big = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.getFrame(p_frame)->setDelayTime((unsigned int)((big<<8) + little));
    if(gif.getFrame(p_frame)->getTranspColorFlag() == 1){
        gif.getFrame(p_frame)->setTranspColorIndex(getBit(m_fileContent[p_pointer++],0,8));
    }
    p_pointer++; //end-command "00"
}

/**
 * @brief
 *
 * @param p_output
 * @param p_pointer
 * @param p_frame
 */
void IO::setGCE(char *p_output, int &p_pointer, int p_frame)
{
    p_output[p_pointer++] = 2*16+1; //extension introducer (Hex:21)
    p_output[p_pointer++] = 15*16+9; //GCE labe (Hex:F9)
    p_output[p_pointer++] = 4; //blockSize
    char transColorFlag = 0;
    p_output[p_pointer++] = transColorFlag; //PackedField
    p_output[p_pointer++] = 0; //delayTime, only in animated (little endian)...
    p_output[p_pointer++] = 0; //...delaTime
    p_output[p_pointer++] = 0; //transparentColorIdenx
    p_output[p_pointer++] = 0; //Block terminator (always Hex: '00')
}

/**
 * @brief
 *
 * @param p_pointer
 * @param p_frame
 */
void IO::getIDiscr(int &p_pointer, int p_frame){
    int leftLittle = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    int leftBig = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.getFrame(p_frame)->setLeft((leftBig<<8)+leftLittle);

    unsigned int rightLittle = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    unsigned int rightBig = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.getFrame(p_frame)->setTop((unsigned int)(rightBig<<8)+rightLittle);

    unsigned int widthLitte = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    unsigned int widthBig = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.getFrame(p_frame)->setWidth((unsigned int)(widthBig<<8) + widthLitte);

    int heightLitte = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    int heightBig = 255 & (unsigned int)(m_fileContent[p_pointer++]);
    gif.getFrame(p_frame)->setHeight((unsigned int)(heightBig<<8) + heightLitte);

    int packedByte = getNextByte(p_pointer);
    gif.getFrame(p_frame)->setLctFlag((unsigned int)getBit(packedByte, 7, 1));
    gif.getFrame(p_frame)->setInterlaceFlag((unsigned int)getBit(packedByte, 6, 1));
    gif.getFrame(p_frame)->setSortFlag((unsigned int)getBit(packedByte, 5, 1));
    if(gif.getFrame(p_frame)->getLctFlag() == 1){
        gif.getFrame(p_frame)->setSizeOfLCT((unsigned int)zweiHochX(getBit(packedByte, 0, 3)+1));
    }
}

/**
 * @brief
 *
 * @param p_output
 * @param p_pointer
 * @param p_frame
 */
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

/**
 * @brief
 *
 * @param p_pointer
 * @param p_frame
 */
void IO::getIData(int &p_pointer, int p_frame){
//    cout << "data" << endl;
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
    m_codeTable = new unsigned char[k];
    int j = 0;
    while(next > 0){
        int blockSize = next;
        for(int i = 0; i<blockSize; i++){ //Byte for Byte
            unsigned int byte = getBit(getNextByte(p_pointer), 0, 8);
            m_codeTable[j++] = getBinChar(byte);
        }
        next = getBit(getNextByte(p_pointer), 0, 8);
    }
    gif.getFrame(p_frame)->setCodeTable(m_codeTable, gif.getFrame(p_frame)->getSizeOfCodeTable());
}

/**
 * @brief
 *
 * @param p_output
 * @param p_pointer
 * @param p_frame
 */
void IO::setIData(char *p_output, int &p_pointer, int p_frame)
{
    p_output[p_pointer++] = gif.getFrame(p_frame)->getMinCodeSize(); //min Code Size
    int bytes = gif.getFrame(p_frame)->getSizeOfCodeTable();
    for(int i = 0; i<bytes; ++i){
        if(i%256 == 0){
            p_output[p_pointer++] = bytes-i>255?255:bytes-i; //number of bytes in this block
        }
        p_output[p_pointer++] = gif.getFrame(p_frame)->getCodeTable()[i]; //codes
    }
    p_output[p_pointer++] = 0; //block terminator
}

/**
 * @brief
 *
 * @param p_output
 * @param p_pointer
 */
void IO::setTrailer(char *p_output, int &p_pointer)
{
    p_output[p_pointer++] = 3*16+11;
}

/**
 * @brief
 *
 * @param p_fileName
 * @param p_content
 * @param p_size
 */
void IO::   getFile(char* p_fileName, char *p_content, int p_size)
{
    fstream file;
    file.open(p_fileName, ios::in);
    file.read(p_content, p_size);
    file.close();
}

/**
 * @brief
 *
 * @param p_fileName
 * @param p_output
 * @param p_fileSize
 */
void IO::saveFile(char* p_fileName, char *p_output, int p_fileSize)
{
    fstream file;
    file.open(p_fileName, ios::out);
    file.write(p_output, p_fileSize);
    file.close();
}

/**
 * @brief
 *
 * @param p_gif
 * @param p_codeTable
 */
void IO::generateColorTable(Gif &p_gif, vector<char> &p_codeTable)
{
    vector<char> color;
    int sizeOfColorTable = 0;
    for(int i = 0; i<p_gif.getHeight()*p_gif.getWidth()*3; i+=3){
        int pos = isColorInTable(p_gif.getPixel(), i, color);
        if(pos == -1){
            if(color.size()<256*3){
                color.push_back(p_gif.getPixel()[i]);
                color.push_back(p_gif.getPixel()[i+1]);
                color.push_back(p_gif.getPixel()[i+2]);
                sizeOfColorTable++;
            }
            p_codeTable.push_back((char)(sizeOfColorTable-1));
        } else {
            p_codeTable.push_back((char)(pos));
        }
    }
    int exponent = 1;
    unsigned int size = zweiHochX(exponent+1);
    while(color.size()/3 > size){
        exponent++;
        size = zweiHochX(exponent+1);
    }
    for(unsigned int i = color.size()/3; i<size; ++i){
        color.push_back((char)0);
        color.push_back((char)0);
        color.push_back((char)0);
    }
    p_gif.setColorTable(color, color.size());
    p_gif.setSizeOfGCT(size);
    p_gif.setGctFlag(1);
}

/**
 * @brief
 *
 * @param pointer
 */
void IO::getCommEx(int &pointer)
{
    int next = getBit(getNextByte(pointer), 0, 8);
    while(next != 0){
        int blocksize = next;
        cout << "comment:";
        for(int i = 0; i<blocksize; ++i){
            int byte = getBit(getNextByte(pointer), 0, 8);
            cout << (char)byte;
        }
        cout << endl;
        next = getBit(getNextByte(pointer), 0, 8);
    }
}

/**
 * @brief
 *
 * @param pointer
 */
void IO::getAppEx(int &pointer)
{
    int next = getBit(getNextByte(pointer), 0, 8);
    cout << "app: " << next << endl;
    for(int i = 0; i < next; ++i){
        int byte = getBit(getNextByte(pointer), 0, 8);
        cout << (char)byte;
    }
    cout << endl;
    cout << "content:" << endl;
    next = getBit(getNextByte(pointer), 0, 8);
    while(next != 0){
        int blocksize = next;
        cout << "block: " << blocksize << ": ";
        for(int i = 0; i<blocksize; ++i){
            int byte = getBit(getNextByte(pointer), 0, 8);
            cout << byte << ", ";
        }
        next = getBit(getNextByte(pointer), 0, 8);
    }
    cout << endl;
}

/**
 * @brief
 *
 * @param pixel
 * @param n
 * @param color
 * @return int
 */
int isColorInTable(char* pixel, int n, vector<char> color){
    for(unsigned int i = 0; i<color.size(); i+=3){
        if(color.at(i) == pixel[n] && color.at(i+1) == pixel[n+1] && color.at(i+2) == pixel[n+2]){
            return i/3;
        }
    }
    return -1;
}

/**
 * @brief
 *
 * @param img
 */
void IO::decompress(int img)
{
    int countPixel = gif.getFrame(img)->getWidth()*gif.getFrame(img)->getHeight()*3;
    if(gif.getFrame(img)->getLctFlag() == 1 && gif.getFrame(img)->getSizeOfLCT() > 2){
        //with LCT
//        cout << "LCT" << endl;
//        cout << "size of LCT: " << gif.getFrame(img)->getSizeOfLCT() << "Flag: " << gif.getFrame(img)->getLctFlag() << endl;
        gif.getFrame(img)->setPixel(LZW::decode(gif.getFrame(img)->getCodeTable(), gif.getFrame(img)->getSizeOfCodeTable(), gif.getFrame(img)->getLct(), gif.getFrame(img)->getSizeOfLCT(), 1, countPixel), countPixel);
    } else {
        //with GCT
//        cout << "GCT" << endl;
        gif.getFrame(img)->setPixel(LZW::decode(gif.getFrame(img)->getCodeTable(), gif.getFrame(img)->getSizeOfCodeTable(), gif.getColorTable(), gif.getSizeOfGCT(), 1, countPixel),countPixel);
    }
}

/**
 * @brief
 *
 * @param p_output
 * @param p_pointer
 */
void IO::setHeader(char *p_output, int &p_pointer)
{
    p_output[p_pointer++] = 'G';
    p_output[p_pointer++] = 'I';
    p_output[p_pointer++] = 'F';
    p_output[p_pointer++] = '8';
    p_output[p_pointer++] = '9';
    p_output[p_pointer++] = 'a';
}

/**
 * @brief
 *
 * @param p_filePath
 */
IO::IO(string p_filePath)
{
    m_fileName = p_filePath;
    m_gce = m_par = m_pte = m_appEx = m_commEx = 0;
    m_fileContent = NULL;
    m_uncompCodeTable = NULL;
    m_output = NULL;
    m_lctTable = NULL;
    m_gctTable = NULL;
    m_codeTable = NULL;
}

/**
 * @brief
 *
 */
IO::IO() {
    m_fileName = "";
    m_gce = m_par = m_pte = m_appEx = m_commEx = 0;
    m_fileContent = NULL;
    m_uncompCodeTable = NULL;
    m_output = NULL;
    m_lctTable = NULL;
    m_gctTable = NULL;
    m_codeTable = NULL;
}

/**
 * @brief
 *
 */
IO::~IO()
{
    delete[] m_fileContent;
    delete[] m_uncompCodeTable;
    //delete[] m_output;
    delete[] m_lctTable;
    delete[] m_gctTable;
    delete[] m_codeTable;
}

/**
 * @brief
 *
 */
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
//    gif.setBgRed(gif.getColorTable()[gif.getBgColor()*3]);
//    gif.setBgGreen(gif.getColorTable()[gif.getBgColor()*3+1]);
//    gif.setBgBlue(gif.getColorTable()[gif.getBgColor()*3+2]);
    cout << "gct size: " << gif.getSizeOfGCT() << endl;
//    cout << "bgcolor: " << getBit(gif.getColorTable()[gif.getBgColor()*3],0,8) << " " << getBit(gif.getColorTable()[gif.getBgColor()*3+1],0,8) << " " <<  getBit(gif.getColorTable()[gif.getBgColor()*3+2],0,8) << endl;
    m_gce = m_pte = m_appEx = m_commEx = 0;
    int next = getBit(getNextByte(pointer), 0, 8);
    int img = 0;
    while(next != 59){ //inner Loop until Trailer: 59 == '3B'
        if(next == 33){ //extension
            next = getBit(getNextByte(pointer), 0, 8);
            if(next == 255){
                //Application Extension
                cout << "app ext" << endl;
                getAppEx(pointer);
//                next = getBit(getNextByte(pointer), 0, 8);
//                pointer += next;
//                next = getBit(getNextByte(pointer), 0, 8);
//                while(next != 0){
//                    pointer += next;
//                    next = getBit(getNextByte(pointer), 0, 8);
//                }
                next = getBit(getNextByte(pointer), 0, 8);
                cout << "next nach app: " << next << endl;
            } else if(next == 254){
                //Comment Extension
                cout << "comm ext" << endl;
                getCommEx(pointer);
//                next = getBit(getNextByte(pointer), 0, 8);
//                while(next != 0){
//                    pointer += next;
//                    next = getBit(getNextByte(pointer), 0, 8);
//                }
                next = getBit(getNextByte(pointer), 0, 8);
            } else if(next == 249){ //Graphic Control Extension
                cout << "gce" << endl;
                if(img == gif.getSizeOfFrames()){
                    gif.extendFrames();
//                    cout << "size frames: " << gif.getSizeOfFrames() << endl;
                }
                m_gce = 1;
                getGCE(pointer, img);
                next = getBit(getNextByte(pointer), 0, 8);
            } else  if(next == 1 || next == 33){
                next = getBit(getNextByte(pointer), 0, 8);
                if(next == 1){
                    //Plain Text Extension
                    cout << "pte" << endl;
                    next = getBit(getNextByte(pointer), 0, 8);
                    pointer += next;
                    next = getBit(getNextByte(pointer), 0, 8);
                    while(next != 0){
                        pointer += next;
                        next = getBit(getNextByte(pointer), 0, 8);
                    }
                    next = getBit(getNextByte(pointer), 0, 8);
                }
            }
        } else if(next == 44){
            cout << "imageDiscriptor" << endl;
            if(img == gif.getSizeOfFrames()){
                gif.extendFrames();
            }
            getIDiscr(pointer, img);
            if(gif.getFrame(img)->getLctFlag() == 1)
                getLCT(pointer, img);
            getIData(pointer, img);
            img++;
            next = getBit(getNextByte(pointer), 0, 8);
        } else {
            next = getBit(getNextByte(pointer), 0, 8);
        }
    }
    // LZW decompression
    for(int i = 0; i<gif.getSizeOfFrames(); ++i){
        decompress(i);
        if(gif.getFrame(i)->getInterlaceFlag() == 1){
            gif.getFrame(i)->setPixel(InterlacedPicture::getUninterlacedPicture(gif.getFrame(i)->getPixel(), gif.getFrame(i)->getWidth(), gif.getFrame(i)->getHeight()), gif.getFrame(i)->getSizeOfPixel());
        }
    }
}

/**
 * @brief
 *
 */
void IO::generateFile()
{
    //first get the size of bytes to generate the file-char-array
    int fileSize = 13; //filesize: size of bytes for the output-file; header+screenDescription = 13
    fileSize += 8; //GCE
    fileSize += 10; //ImageDiscriptor

    int sizeOfCodeTable = 0;
    vector<char> codeTable; //sequenze of colors (uncompressed)
    generateColorTable(gif, codeTable); //generate ColorTable and set codeTable
    m_uncompCodeTable = new char[codeTable.size()];
    for(unsigned int j = 0; j<codeTable.size(); ++j){
        m_uncompCodeTable[j] = codeTable.at(j);
    }
    unsigned char *codes = LZW::encode(m_uncompCodeTable, codeTable.size(), gif.getColorTable(), gif.getSizeOfGCT(), sizeOfCodeTable);
    gif.extendFrames();
    gif.getFrame(0)->setWidth(gif.getWidth());
    gif.getFrame(0)->setHeight(gif.getHeight());
    gif.getFrame(0)->setLeft(0);
    gif.getFrame(0)->setTop(0);
    gif.getFrame(0)->setLctFlag(0);
    gif.getFrame(0)->setCodeTable(codes, sizeOfCodeTable);
    gif.getFrame(0)->setMinCodeSize(log2(gif.getSizeOfGCT()));
    int blocks = sizeOfCodeTable/256 + (sizeOfCodeTable%256>0?1:0);
    fileSize += 1 + blocks + sizeOfCodeTable + 1; //minCodeSize + blockinfos + blockcontents + Block terminator
    fileSize += gif.getSizeOfGCT()*3;
    fileSize += 1; //Trailer

    m_output = new char[fileSize];
    int pointer = 0;
    setHeader(m_output, pointer);
    setScreen(m_output, pointer);
    if(gif.getGctFlag()){
        setGCT(m_output, pointer);
    }
    setGCE(m_output, pointer, 0);
    setIDiscr(m_output, pointer, 0);
    if(gif.getFrame(0)->getLctFlag()){
        setLCT(m_output, pointer, 0);
    }
    setIData(m_output, pointer, 0);
    setTrailer(m_output, pointer);
    saveFile((char*)m_fileName.c_str(), m_output, fileSize);
}

/**
 * @brief
 *
 * @return Gif
 */
Gif *IO::getGif()
{
    return &gif;
}

/**
 * @brief
 *
 * @param gif
 */
void IO::setGif(Gif gif)
{
    this->gif = gif;
}

/**
 * @brief
 *
 * @param p_wert
 * @param p_bit
 * @param p_count
 * @return int
 */
int IO::getBit(int p_wert, int p_bit, int p_count){
   return (p_wert & ((zweiHochX(p_count)-1) << p_bit))>>p_bit;
}

/**
 * @brief
 *
 * @param p_exponent
 * @return int
 */
int IO::zweiHochX(int p_exponent){
    int res = 1;
    for(int i = 0; i<p_exponent; i++)
        res *= 2;
    return res;
}

/**
 * @brief
 *
 * @param p_binary
 * @return char
 */
char IO::getHex(int p_binary){
    if(p_binary>9)
        return static_cast<char>(p_binary+55);
    else
        return static_cast<char>(p_binary+48);
}

/**
 * @brief
 *
 * @param p_binary
 * @return unsigned char
 */
unsigned char IO::getBinChar(unsigned int p_binary){
    return (unsigned char)getBit(p_binary, 0, 8);
}

/**
 * @brief
 *
 * @param p_pointer
 * @return unsigned int
 */
unsigned int IO::getNextByte(int &p_pointer){
    return (unsigned int)m_fileContent[p_pointer++];
}

/**
 * @brief
 *
 * @param s
 * @return int
 */
int get_size(const char* s){
   FILE* file = fopen(s, "rb");
   fseek(file, 0L, SEEK_END);
   int size = ftell(file);
   fclose(file);
   return size;
}
