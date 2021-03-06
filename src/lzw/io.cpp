#include "io.h"
#include "lzw.h"
#include "gif.h"
#include "interlacedpicture.h"
#include "median_cut.h"
#include "tableconverter.h"

#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <assert.h>
#include<cstdint>

using namespace std;



int get_size(const char *file);

int isColorInTable(unsigned char *pixel, int n, vector<unsigned char> color);

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

void IO::getGCT(int &p_pointer){
    m_GCT = new unsigned char[gif.getSizeOfGCT()*3];
    for(int i = 0; i<gif.getSizeOfGCT(); ++i){
        m_GCT[i*3] = m_fileContent[p_pointer++];
        m_GCT[i*3+1] = m_fileContent[p_pointer++];
        m_GCT[i*3+2] = m_fileContent[p_pointer++];
    }
    gif.setGCT(m_GCT, gif.getSizeOfGCT());
}

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

void IO::getFile(char* p_fileName, char *p_content, int p_size)
{
    fstream file;
    file.open(p_fileName, ios::in);
    file.read(p_content, p_size);
    file.close();
}

void IO::saveFile(char* p_fileName, unsigned char *p_output, int p_fileSize)
{
    fstream file;
	file.open(p_fileName, ios::out | ios::binary);
	file.write((char*)p_output, p_fileSize);
    file.close();
}

void IO::generateRawData(Gif &p_gif, int p_frame, int p_withColorTable)
{
	vector<unsigned char> color;
		if(p_withColorTable == 0){ //keep existing color table
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
		else if (p_withColorTable == 1){ //use median-cut


				std::vector<Point> points;

				for (int j = 0; j < p_gif.getFrame(p_frame)->getHeight() * p_gif.getFrame(p_frame)->getWidth()*3; j+=3) {
					Point curr;
					curr.x[0] = p_gif.getFrame(p_frame)->getPixel()[j];
					curr.x[1] = p_gif.getFrame(p_frame)->getPixel()[j+1];
					curr.x[2] = p_gif.getFrame(p_frame)->getPixel()[j+2];

					points.push_back(curr);
				}

				std::vector<Point> reducedTable = medianCut(points.data(),points.size(),points.size() > 256?256:points.size());

				std::map<Point, int> lookup;
				int r, g, b, minEuclidDistIndex = 0;
				double euclidDist = 0, minEuclidDist = 500;


				for (int j = 0; j < p_gif.getFrame(p_frame)->getHeight() * p_gif.getFrame(p_frame)->getWidth()*3; j+=3){

					//find best matching color
					euclidDist = 0;
					minEuclidDist = 500;
					minEuclidDistIndex = 0;

					Point curr;
					curr.x[0] = p_gif.getFrame(p_frame)->getPixel()[j];
					curr.x[1] = p_gif.getFrame(p_frame)->getPixel()[j+1];
					curr.x[2] = p_gif.getFrame(p_frame)->getPixel()[j+2];

					if(lookup.count(curr) == 0){ //not in lookup table
						for (int k = 0; k < (int)reducedTable.size(); ++k) {
							r = curr.x[0] - reducedTable[k].x[0];
							g = curr.x[1] - reducedTable[k].x[1];
							b = curr.x[2] - reducedTable[k].x[2];
							euclidDist = sqrt(r*r + g*g + b*b);

							if(euclidDist < minEuclidDist){
								minEuclidDist = euclidDist;
								minEuclidDistIndex = k;
							}
						}
						lookup[curr] = minEuclidDistIndex;
					}
					else{
						minEuclidDistIndex = lookup[curr];
					}

					//apply color
					p_gif.getFrame(p_frame)->getPixel()[j] = reducedTable[minEuclidDistIndex].x[0];
					p_gif.getFrame(p_frame)->getPixel()[j+1] = reducedTable[minEuclidDistIndex].x[1];
					p_gif.getFrame(p_frame)->getPixel()[j+2] = reducedTable[minEuclidDistIndex].x[2];
				}

				unsigned char* newTable = TableConverter::createTableArray(reducedTable);
				p_gif.setGctFlag(0);
				p_gif.getFrame(p_frame)->setTranspColorFlag(0);
				p_gif.getFrame(p_frame)->setLctFlag(1);
				p_gif.getFrame(p_frame)->setLct(newTable, reducedTable.size());

		}

		vector<unsigned char> rawData;
		int sizeOfColorTable = 0;
		int pos = -1;
		for(int i = 0; i<p_gif.getFrame(p_frame)->getHeight()*p_gif.getFrame(p_frame)->getWidth()*3; i+=3){
			pos = isColorInTable(p_gif.getFrame(p_frame)->getPixel(), i, color);
			if(p_withColorTable == 2 && pos == -1){
                if(color.size()<256*3){
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


		if(p_withColorTable == 2){

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

int isColorInTable(unsigned char* pixel, int n, vector<unsigned char> color){
    for(unsigned int i = 0; i<color.size(); i+=3){
        if(color.at(i) == pixel[n] && color.at(i+1) == pixel[n+1] && color.at(i+2) == pixel[n+2]){
            return i/3;
        }
    }
    return -1;
}

void IO::decompress(int p_frame)
{

	try{
		int sizeOfData = gif.getFrame(p_frame)->getWidth()*gif.getFrame(p_frame)->getHeight();
		gif.getFrame(p_frame)->setData(m_lzw.decode(gif, p_frame), sizeOfData);
		generatePixel(gif, p_frame);
	}
	catch(...){std::cout<<"ha, caught something!.. an error :("<<std::endl<<std::flush;}
}

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

IO::~IO()
{
    delete[] m_fileContent;
    delete[] m_rawData;
    //delete[] m_output;
    delete[] m_LCT;
    delete[] m_GCT;
    //delete[] m_compData;
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
            next = getBit(getNextByte(pointer), 0, 8);
            if(next == 255){
                //Application Extension
				///cout << "app ext" << endl;
                getAppEx(pointer);
                next = getBit(getNextByte(pointer), 0, 8);
				///cout << "next nach app: " << next << endl;
            } else if(next == 254){
                //Comment Extension
				///cout << "comm ext" << endl;
                getCommEx(pointer);
                next = getBit(getNextByte(pointer), 0, 8);
            } else if(next == 249){ //Graphic Control Extension
				///cout << "gce" << endl;
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
					///cout << "pte" << endl;
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

		generateRawData(p_gif, i, 0); //generate ColorTable and set codeTable
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

Gif *IO::getGif()
{
    return &gif;
}

void IO::setGif(Gif& gif)
{
    this->gif = gif;
}

int IO::getBit(int p_wert, int p_bit, int p_count){
   return (p_wert & ((zweiHochX(p_count)-1) << p_bit))>>p_bit;
}

int IO::zweiHochX(int p_exponent){
    return 2<<(p_exponent-1);
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
