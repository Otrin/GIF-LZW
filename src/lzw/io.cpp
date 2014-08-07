#include "io.h"
#include "lzw.h"
#include "gif.h"
#include "interlacedpicture.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <assert.h>
#include<cstdint>

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
int isColorInTable(unsigned char *pixel, int n, vector<unsigned char> color);

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
void IO::setScreen(Gif& p_gif, char *p_output, int &p_pointer)
{
	p_output[p_pointer++] = (char)(p_gif.getWidth()%256); //gif width (little endian)
	p_output[p_pointer++] = (char)(p_gif.getWidth() - (p_gif.getWidth()%256)); //gif width
	p_output[p_pointer++] = (char)(p_gif.getHeight()%256); //gif height (little endian)
	p_output[p_pointer++] = (char)(p_gif.getHeight() - (p_gif.getHeight()%256)); //gif height
    int gctFlag = 0;
	if(p_gif.getGctFlag() == 1){
        gctFlag = 1<<7;
    }
    int colorRes = 1<<4;
	int sizeOfGCT = log2(p_gif.getSizeOfGCT())-1;
    unsigned char packed = (unsigned char)(gctFlag | colorRes | sizeOfGCT);
    p_output[p_pointer++] = packed; //packed Byte
	p_output[p_pointer++] = (char)p_gif.getBgColor(); //Background color index
	p_output[p_pointer++] = (char)0; //Pixel Aspect Ratio
}

/**
 * @brief
 *
 * @param p_pointer
 * @param p_frame
 */
void IO::getLCT(int &p_pointer, int p_frame){
//    cout << "LCT " << p_frame << endl;
    m_LCT = new unsigned char[gif.getFrame(p_frame)->getSizeOfLCT()*3];
    for(int i = 0; i<gif.getFrame(p_frame)->getSizeOfLCT(); ++i){
        m_LCT[i*3] = m_fileContent[p_pointer++];
        m_LCT[i*3+1] = m_fileContent[p_pointer++];
        m_LCT[i*3+2] = m_fileContent[p_pointer++];
    }
    gif.getFrame(p_frame)->setLct(m_LCT, gif.getFrame(p_frame)->getSizeOfLCT());
}

/**
 * @brief
 *
 * @param p_output
 * @param p_pointer
 * @param p_frame
 */
void IO::setLCT(Gif& p_gif, char *p_output, int &p_pointer, int p_frame)
{
	for(int i = 0; i<p_gif.getFrame(p_frame)->getSizeOfLCT()*3; ++i){ //last 2 codes not in file!
		p_output[p_pointer++] = p_gif.getFrame(p_frame)->getLct()[i]; //lines of Colors
    }
}

/**
 * @brief
 *
 * @param p_pointer
 */
void IO::getGCT(int &p_pointer){
    m_GCT = new unsigned char[gif.getSizeOfGCT()*3];
    for(int i = 0; i<gif.getSizeOfGCT(); ++i){
        m_GCT[i*3] = m_fileContent[p_pointer++];
        m_GCT[i*3+1] = m_fileContent[p_pointer++];
        m_GCT[i*3+2] = m_fileContent[p_pointer++];
    }
    gif.setGCT(m_GCT, gif.getSizeOfGCT());
}

/**
 * @brief
 *
 * @param p_output
 * @param p_pointer
 */
void IO::setGCT(Gif& p_gif, char *p_output, int &p_pointer)
{
	for(int i = 0; i<p_gif.getSizeOfGCT()*3; ++i){ //last 2 codes not in file!
		p_output[p_pointer++] = p_gif.getGCT()[i]; //lines of Colors
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
void IO::setGCE(Gif& p_gif, char *p_output, int &p_pointer, int p_frame)
{
	p_output[p_pointer++] = (char)(0x21); //extension introducer (Hex:21)
	p_output[p_pointer++] = (char)(0xf9); //GCE labe (Hex:F9)
	p_output[p_pointer++] = (char)(0x04); //blockSize

	p_output[p_pointer++] = (char)p_gif.getFrame(p_frame)->getTranspColorFlag(); //PackedField
	p_output[p_pointer++] = (char)(p_gif.getFrame(p_frame)->getDelayTime()%256);
	p_output[p_pointer++] = (char)(p_gif.getFrame(p_frame)->getDelayTime() - (p_gif.getFrame(p_frame)->getDelayTime()%256));
	p_output[p_pointer++] = (char)p_gif.getFrame(p_frame)->getTranspColorIndex(); //transparentColorIdenx
	p_output[p_pointer++] = (char)0x00; //Block terminator (always Hex: '00')
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
void IO::setIDiscr(Gif& p_gif, char *p_output, int &p_pointer, int p_frame)
{
    p_output[p_pointer++] = 2*16+12; //Image separator, always Hex: '2c'
	p_output[p_pointer++] = p_gif.getFrame(p_frame)->getLeft()%256; //image left (little endian)
	p_output[p_pointer++] = p_gif.getFrame(p_frame)->getLeft() - (p_gif.getFrame(p_frame)->getLeft()%256); //image left

	p_output[p_pointer++] = p_gif.getFrame(p_frame)->getTop()%256; //image top (little endian)
	p_output[p_pointer++] = p_gif.getFrame(p_frame)->getTop() - (p_gif.getFrame(p_frame)->getTop()%256); //image top

	p_output[p_pointer++] = p_gif.getFrame(p_frame)->getWidth()%256; //image width (little endian)
	p_output[p_pointer++] = p_gif.getFrame(p_frame)->getWidth() - (p_gif.getFrame(p_frame)->getWidth()%256); //image width

	p_output[p_pointer++] = p_gif.getFrame(p_frame)->getHeight()%256; //image height (little endian)
	p_output[p_pointer++] = p_gif.getFrame(p_frame)->getHeight() - (p_gif.getFrame(p_frame)->getHeight()%256); //image height

	char lctFlag = p_gif.getFrame(p_frame)->getLctFlag()?(1<<7):0;
	char sizeOfLCT = log2(p_gif.getFrame(p_frame)->getSizeOfLCT()-2)-1;
    p_output[p_pointer++] = lctFlag | sizeOfLCT; //packed Byte
}

/**
 * @brief
 *
 * @param p_pointer
 * @param p_frame
 */
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
    m_compData = new unsigned char[k];
    int j = 0;
    while(next > 0){
        int blockSize = next;
        for(int i = 0; i<blockSize; i++){ //Byte for Byte
            unsigned int byte = getBit(getNextByte(p_pointer), 0, 8);
            m_compData[j++] = getBit(byte, 0, 8);
        }
        next = getBit(getNextByte(p_pointer), 0, 8);
    }
    gif.getFrame(p_frame)->setData(m_compData, gif.getFrame(p_frame)->getSizeOfData());
    gif.getFrame(p_frame)->setDataFlag(1);
}

/**
 * @brief
 *
 * @param p_output
 * @param p_pointer
 * @param p_frame
 */
void IO::setIData(Gif& p_gif, char *p_output, int &p_pointer, int p_frame)
{
	p_output[p_pointer++] = p_gif.getFrame(p_frame)->getMinCodeSize(); //min Code Size
	int bytes = p_gif.getFrame(p_frame)->getSizeOfData();
    for(int i = 0; i<bytes; ++i){
        if(i%256 == 0){
            p_output[p_pointer++] = bytes-i>255?255:bytes-i; //number of bytes in this block
        }
		p_output[p_pointer++] = p_gif.getFrame(p_frame)->getData()[i]; //codes
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
	p_output[p_pointer++] = (char)0x3b;
}

/**
 * @brief
 *
 * @param p_fileName
 * @param p_content
 * @param p_size
 */
void IO::getFile(char* p_fileName, char *p_content, int p_size)
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
void IO::saveFile(char* p_fileName, unsigned char *p_output, int p_fileSize)
{
    fstream file;
	file.open(p_fileName, ios::out | ios::binary);
	file.write((char*)p_output, p_fileSize);
    file.close();
}

/**
 * @brief
 *
 * @param p_gif the gif-object
 * @param p_frame numer on Frame, which will be generate
 * @param p_withColorTable flag, if you want to generate a colorTable oder not. if 1, then it generates a LCT.
 */
void IO::generateRawData(Gif &p_gif, int p_frame, bool p_withColorTable)
{
    vector<unsigned char> color;
    if(!p_withColorTable){
        if(p_gif.getFrame(p_frame)->getLctFlag() == 1){
            for (int i = 0; i < p_gif.getFrame(p_frame)->getSizeOfLCT(); ++i) {
                color.push_back(p_gif.getFrame(p_frame)->getLct()[i*3]);
                color.push_back(p_gif.getFrame(p_frame)->getLct()[i*3+1]);
                color.push_back(p_gif.getFrame(p_frame)->getLct()[i*3+2]);
            }
        } else {
            for (int i = 0; i < p_gif.getSizeOfGCT(); ++i) {
                color.push_back(p_gif.getGCT()[i*3]);
                color.push_back(p_gif.getGCT()[i*3+1]);
                color.push_back(p_gif.getGCT()[i*3+2]);
            }
        }
    }
    vector<unsigned char> rawData;
    int sizeOfColorTable = 0;
    int pos = -1;
    for(int i = 0; i<p_gif.getFrame(p_frame)->getHeight()*p_gif.getFrame(p_frame)->getWidth()*3; i+=3){
        pos = isColorInTable(p_gif.getFrame(p_frame)->getPixel(), i, color);
        if(pos == -1){
            if(p_withColorTable && color.size()<256*3){
                color.push_back(p_gif.getFrame(p_frame)->getPixel()[i]);
                color.push_back(p_gif.getFrame(p_frame)->getPixel()[i+1]);
                color.push_back(p_gif.getFrame(p_frame)->getPixel()[i+2]);
                sizeOfColorTable++;
            }
            rawData.push_back((unsigned char)(sizeOfColorTable-1));
        } else {
            rawData.push_back((unsigned char)(pos));
        }
    }

	if(p_withColorTable){
		int exponent = 1;
		unsigned int size = zweiHochX(exponent+1);
		while(color.size()/3 > size){
			exponent++;
			size = zweiHochX(exponent+1);
		}
		for(unsigned int i = color.size()/3; i<size; ++i){
			color.push_back((unsigned char)0);
			color.push_back((unsigned char)0);
			color.push_back((unsigned char)0);
		}
		p_gif.getFrame(p_frame)->setLct(color, color.size());
		p_gif.getFrame(p_frame)->setLctFlag(1);
		p_gif.setGctFlag(0);
	}
	p_gif.getFrame(p_frame)->setData(rawData);
	p_gif.getFrame(p_frame)->setDataFlag(0);
}

/**
 * @brief
 *
 * @param p_gif
 * @param p_frame
 */
void IO::generatePixel(Gif &p_gif, int p_frame)
{
    p_gif.getFrame(p_frame)->setSizeOfPixel(p_gif.getFrame(p_frame)->getHeight()*p_gif.getFrame(p_frame)->getWidth()*3);
    unsigned char *pixel = new unsigned char[p_gif.getFrame(p_frame)->getSizeOfPixel()];
    for (int i = 0; i < p_gif.getFrame(p_frame)->getSizeOfData(); ++i) {
        if(p_gif.getFrame(p_frame)->getLctFlag() == 1){
            pixel[i*3] = p_gif.getFrame(p_frame)->getLct()[p_gif.getFrame(p_frame)->getData()[i]*3];
            pixel[i*3+1] = p_gif.getFrame(p_frame)->getLct()[p_gif.getFrame(p_frame)->getData()[i]*3+1];
            pixel[i*3+2] = p_gif.getFrame(p_frame)->getLct()[p_gif.getFrame(p_frame)->getData()[i]*3+2];
        } else {
            pixel[i*3] = p_gif.getGCT()[p_gif.getFrame(p_frame)->getData()[i]*3];
            pixel[i*3+1] = p_gif.getGCT()[p_gif.getFrame(p_frame)->getData()[i]*3+1];
            pixel[i*3+2] = p_gif.getGCT()[p_gif.getFrame(p_frame)->getData()[i]*3+2];
        }
    }
    p_gif.getFrame(p_frame)->setPixel(pixel, p_gif.getFrame(p_frame)->getSizeOfPixel());
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
int isColorInTable(unsigned char* pixel, int n, vector<unsigned char> color){
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
void IO::decompress(int p_frame)
{

	try{
		int sizeOfData = gif.getFrame(p_frame)->getWidth()*gif.getFrame(p_frame)->getHeight();
		gif.getFrame(p_frame)->setData(m_lzw.decode(gif, p_frame), sizeOfData);
		generatePixel(gif, p_frame);
	}
	catch(...){std::cout<<"ha, caught something!.. an error :("<<std::endl<<std::flush;}
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
    m_rawData = NULL;
    m_output = NULL;
    m_LCT = NULL;
    m_GCT = NULL;
    m_compData = NULL;
}

/**
 * @brief
 *
 */
IO::IO() {
    m_fileName = "";
    m_gce = m_par = m_pte = m_appEx = m_commEx = 0;
    m_fileContent = NULL;
    m_rawData = NULL;
    m_output = NULL;
    m_LCT = NULL;
    m_GCT = NULL;
    m_compData = NULL;
}

/**
 * @brief
 *
 */
IO::~IO()
{
    delete[] m_fileContent;
    delete[] m_rawData;
    //delete[] m_output;
    delete[] m_LCT;
    delete[] m_GCT;
    //delete[] m_compData;
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
                next = getBit(getNextByte(pointer), 0, 8);
                cout << "next nach app: " << next << endl;
            } else if(next == 254){
                //Comment Extension
                cout << "comm ext" << endl;
                getCommEx(pointer);
                next = getBit(getNextByte(pointer), 0, 8);
            } else if(next == 249){ //Graphic Control Extension
                cout << "gce" << endl;
                if(img == gif.getSizeOfFrames()){
                    gif.extendFrames();
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
	//requires rewrite, purpose: save files from raw data

	/*//first get the size of bytes to generate the file-char-array
    int fileSize = 13; //filesize: size of bytes for the output-file; header+screenDescription = 13
    fileSize += 8; //GCE
    fileSize += 10; //ImageDiscriptor
    gif.setWidth(gif.getFrame(0)->getWidth());
    gif.setHeight(gif.getFrame(0)->getHeight());
    generateRawData(gif, 0, 1); //generate ColorTable and set codeTable
    cout << "sizeofData: " << gif.getFrame(0)->getSizeOfData() << endl;
    m_lzw.encode(gif, 0);
    cout << "sizeofData: " << gif.getFrame(0)->getSizeOfData() << endl;
    gif.getFrame(0)->setMinCodeSize(log2(gif.getSizeOfGCT()));
    int blocks = gif.getFrame(0)->getSizeOfData()/256 + (gif.getFrame(0)->getSizeOfData()%256>0?1:0);
    fileSize += 1 + blocks + gif.getFrame(0)->getSizeOfData() + 1; //minCodeSize + blockinfos + blockcontents + Block terminator
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
	saveFile((char*)m_fileName.c_str(), m_output, fileSize);*/
}

void IO::saveGif(Gif& p_gif){

	std::vector<unsigned char> outputData;

	prepareData(p_gif);

	if(p_gif.getSizeOfFrames() == 1 || checkDelayTime(p_gif)){ //still picture
		writeHeader(outputData, p_gif);
		writeFrames(outputData, p_gif);
		writeTrailer(outputData);
	}
	else if(p_gif.getSizeOfFrames() > 1){ //animated
		writeHeader(outputData, p_gif);
		writeAppExt(outputData);
		writeFrames(outputData, p_gif);
		writeTrailer(outputData);
	}
	else{
		//error
	}

	saveFile((char*)m_fileName.c_str(), outputData.data(), outputData.size());
}



void IO::prepareData(Gif& p_gif){

	for (int i = 0; i < p_gif.getSizeOfFrames(); ++i) {

		std::cout<<"encoding frame "<<i+1<<" of "<<p_gif.getSizeOfFrames()<<std::endl<<std::flush;

		generateRawData(p_gif, i, false); //generate ColorTable and set codeTable
		LZW lzw;
		lzw.encode(p_gif, i);

		if(p_gif.getFrame(i)->getLctFlag()){
			p_gif.getFrame(i)->setMinCodeSize(log2(p_gif.getFrame(i)->getSizeOfLCT()));
		}else{
			p_gif.getFrame(i)->setMinCodeSize(log2(p_gif.getSizeOfGCT()));
		}

		std::cout<<"frame "<<i+1<<" encoding done"<<std::endl<<std::flush;
	}
}

void IO::writeFrames(std::vector<unsigned char>& p_outputData, Gif& p_gif){
	for (int i = 0; i < p_gif.getSizeOfFrames(); ++i) {
		writeGCE(p_outputData,p_gif,i);
		writeImageBlock(p_outputData,p_gif,i);
	}
}

void IO::writeHeader(std::vector<unsigned char>& p_outputData, Gif& p_gif){
	p_outputData.push_back((unsigned char)0x47);//GIF89a
	p_outputData.push_back((unsigned char)0x49);
	p_outputData.push_back((unsigned char)0x46);
	p_outputData.push_back((unsigned char)0x38);
	p_outputData.push_back((unsigned char)0x39);
	p_outputData.push_back((unsigned char)0x61);

	std::uint16_t w, h;

	w = (uint16_t)p_gif.getWidth();
	h = (uint16_t)p_gif.getHeight();

	p_outputData.push_back((unsigned char)(w & 0x00FF));
	p_outputData.push_back((unsigned char)(w>>8 & 0x00FF));

	p_outputData.push_back((unsigned char)(h & 0x00FF));
	p_outputData.push_back((unsigned char)(h>>8 & 0x00FF));

	unsigned char packed = 0x00, mask = 0x07;

	packed |= 0x01<<4; //color resolution

	if(p_gif.getGctFlag() == 1){
		packed |= 1<<7;
		packed |= (((int)(log2(p_gif.getSizeOfGCT()))-1) & mask);
	}else{
		packed |= 1;
	}
	p_outputData.push_back(packed);
	p_outputData.push_back((unsigned char)p_gif.getBgColor());
	p_outputData.push_back((unsigned char) 0x00);

	for (int i = 0; i < p_gif.getSizeOfGCT()*3; i+=3) {
		p_outputData.push_back(p_gif.getGCT()[i]);
		p_outputData.push_back(p_gif.getGCT()[i+1]);
		p_outputData.push_back(p_gif.getGCT()[i+2]);
	}
}

void IO::writeGCE(std::vector<unsigned char> &p_outputData, Gif& p_gif, int p_frame){
	p_outputData.push_back((unsigned char)0x21);
	p_outputData.push_back((unsigned char)0xf9);
	p_outputData.push_back((unsigned char)0x04);

	unsigned char packed = 0x00, disp = (unsigned char)p_gif.getFrame(p_frame)->getDisposualMethod();
	packed |= disp<<2;
	packed |= (unsigned char)p_gif.getFrame(p_frame)->getTranspColorFlag();

	packed |= (unsigned char)p_gif.getFrame(p_frame)->getUserInputFlag()<<1;

	p_outputData.push_back(packed);

	std::uint16_t d;
	d = (uint16_t)p_gif.getFrame(p_frame)->getDelayTime();

	p_outputData.push_back((unsigned char)(d & 0x00FF));
	p_outputData.push_back((unsigned char)(d>>8 & 0x00FF));

	p_outputData.push_back((unsigned char)p_gif.getFrame(p_frame)->getTranspColorIndex());
	p_outputData.push_back((unsigned char)0x00);
}

void IO::writeAppExt(std::vector<unsigned char> &p_outputData){//writed netscape extension
	p_outputData.push_back((unsigned char)0x21);
	p_outputData.push_back((unsigned char)0xff);
	p_outputData.push_back((unsigned char)0x0b);

	p_outputData.push_back((unsigned char)0x4E);//netscape
	p_outputData.push_back((unsigned char)0x45);
	p_outputData.push_back((unsigned char)0x54);
	p_outputData.push_back((unsigned char)0x53);
	p_outputData.push_back((unsigned char)0x43);
	p_outputData.push_back((unsigned char)0x41);
	p_outputData.push_back((unsigned char)0x50);
	p_outputData.push_back((unsigned char)0x45);

	p_outputData.push_back((unsigned char)0x32);//2.0
	p_outputData.push_back((unsigned char)0x2E);
	p_outputData.push_back((unsigned char)0x30);

	p_outputData.push_back((unsigned char)0x03);//sub-block data size
	p_outputData.push_back((unsigned char)0x01);//sub-block id

	p_outputData.push_back((unsigned char)0x00);//loop forever
	p_outputData.push_back((unsigned char)0x00);//^

	p_outputData.push_back((unsigned char)0x00);//trailer
}

void IO::writeImageBlock(std::vector<unsigned char> &p_outputData, Gif& p_gif, int p_frame){
	p_outputData.push_back((unsigned char)0x2c);

	std::uint16_t w, h, l, t;

	w = (uint16_t)p_gif.getFrame(p_frame)->getWidth();
	h = (uint16_t)p_gif.getFrame(p_frame)->getHeight();
	l = (uint16_t)p_gif.getFrame(p_frame)->getLeft();
	t = (uint16_t)p_gif.getFrame(p_frame)->getTop();

	p_outputData.push_back((unsigned char)(l & 0x00FF));
	p_outputData.push_back((unsigned char)(l>>8 & 0x00FF));

	p_outputData.push_back((unsigned char)(t & 0x00FF));
	p_outputData.push_back((unsigned char)(t>>8 & 0x00FF));

	p_outputData.push_back((unsigned char)(w & 0x00FF));
	p_outputData.push_back((unsigned char)(w>>8 & 0x00FF));

	p_outputData.push_back((unsigned char)(h & 0x00FF));
	p_outputData.push_back((unsigned char)(h>>8 & 0x00FF));

	unsigned char packed = 0x00, mask = 0x07;

	if(p_gif.getFrame(p_frame)->getLctFlag() == 1){
		packed |= 1<<7;
		packed |= (((int)(log2(p_gif.getFrame(p_frame)->getSizeOfLCT()))-1) & mask);
	}
	p_outputData.push_back(packed);

	for (int i = 0; i < p_gif.getFrame(p_frame)->getSizeOfLCT()*3; i+=3) {
		p_outputData.push_back(p_gif.getFrame(p_frame)->getLct()[i]);
		p_outputData.push_back(p_gif.getFrame(p_frame)->getLct()[i+1]);
		p_outputData.push_back(p_gif.getFrame(p_frame)->getLct()[i+2]);
	}

	p_outputData.push_back((unsigned char)p_gif.getFrame(p_frame)->getMinCodeSize());//lzw min code size

	//lzw blocks
	int totalSize = p_gif.getFrame(p_frame)->getSizeOfData();
	for(int i = 0; i<totalSize; ++i){
		if(i%255 == 0){
			p_outputData.push_back((unsigned char)((totalSize-i)>255?255:totalSize-i));//blocksize
		}
		p_outputData.push_back(p_gif.getFrame(p_frame)->getData()[i]);
	}

	p_outputData.push_back((unsigned char)0x00);//terminator
}

void IO::writeTrailer(std::vector<unsigned char> &p_outputData){
	p_outputData.push_back((unsigned char)0x3b);
}


bool IO::checkDelayTime(Gif &p_gif)
{
	bool chk = true;

	for (int i = 0; i < p_gif.getSizeOfFrames(); i++) {
		if(p_gif.getFrame(i)->getDelayTime() != 0) chk = false;
	}

	return chk;
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
void IO::setGif(Gif& gif)
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
    return 2<<(p_exponent-1);
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
