#include <map>
#include <math.h>
#include <cstring>

#include<iostream>
#include<fstream>
#include<sstream>
#include<assert.h>

#include "tableconverter.h"
#include "gif.h"
#include "frame.h"

Gif* TableConverter::globalToLocal(const Gif *p_gif) {
	if (!p_gif->getGctFlag())
		return NULL;


	Gif* res = new Gif(*p_gif);

	if (res->getSizeOfFrames() > 1) { //easymode
		char** gcts = copyTableMultiple(res->getColorTable(),res->getSizeOfGCT()*3,res->getSizeOfFrames());

		for (int i = 0; i < res->getSizeOfFrames(); ++i) {
			Frame* frame = res->getFrame(i);
			frame->setLctFlag(1);
			frame->setLct(gcts[i], res->getSizeOfGCT());
		}
		delete[] gcts;
	}
	else{ //hardmode (split stuff, static one frame gifs)

		const int numSegments = 3; //doesn't really work with other formats

		Frame org = *(res->getFrame(0));

		int blockSizeW = org.getWidth()/numSegments;
		int blockSizeH = org.getHeight()/numSegments;
		int marginW = org.getWidth()-blockSizeW*numSegments;
		int marginH = org.getHeight()-blockSizeH*numSegments;


		//add margin to last row
		/*
		*		Pic is divided into 9 subframes
		*      _______________
		*     *    |    |    '|
		*	  *____|____|____'|  \
		*     *    |    |    '|   |extra space
		*     *____|____|____'|  /
		*     *    |    |    '|
		*     *____|____|____'|
		*	   _ _ _ _ _ _ _ _
		*
		*		  \____/
		*		extra space
		*/


		for (int i = 0; i < numSegments*numSegments-1; ++i) {
			res->extendFrames();
		}

		char** lcts = copyTableMultiple(res->getColorTable(),res->getSizeOfGCT()*3,numSegments*numSegments);

		char** pixels = new char*[res->getSizeOfFrames()];

		for (int i = 0; i < res->getSizeOfFrames(); ++i) {

			if((i+1) % numSegments == 0 && i >= (numSegments*(numSegments-1))){
				res->getFrame(i)->setWidth(blockSizeW+marginW);
				res->getFrame(i)->setHeight(blockSizeH+marginH);
			}
			else if((i+1) % numSegments == 0){ //frames with extra width

				res->getFrame(i)->setWidth(blockSizeW+marginW);
				res->getFrame(i)->setHeight(blockSizeH);

			}
			else if(i >= (numSegments*(numSegments-1))){ //frames with extra height
				res->getFrame(i)->setHeight(blockSizeH+marginH);
				res->getFrame(i)->setWidth(blockSizeW);
			}
			else{
				res->getFrame(i)->setHeight(blockSizeH);
				res->getFrame(i)->setWidth(blockSizeW);
			}

			pixels[i] = new char[res->getFrame(i)->getHeight()*res->getFrame(i)->getWidth()*3];

			res->getFrame(i)->setTop((i/numSegments)*blockSizeH + org.getTop());
			res->getFrame(i)->setLeft((i%numSegments)*blockSizeW + org.getLeft());

			res->getFrame(i)->setLctFlag(1);
			res->getFrame(i)->setLct(lcts[i],res->getSizeOfGCT());

			res->getFrame(i)->setTranspColorFlag(org.getTranspColorFlag());
			res->getFrame(i)->setTranspColorIndex(org.getTranspColorIndex());

			res->getFrame(i)->setCodeTable(NULL,0);
			res->getFrame(i)->setUserInputFlag(0);
			res->getFrame(i)->setInterlaceFlag(org.getInterlaceFlag());
			res->getFrame(i)->setDisposualMethod(org.getDisposualMethod());
			res->getFrame(i)->setPixel(NULL,0);
			res->getFrame(i)->setCodeTable(NULL,0);

		}	



		int oInd = 0;
		for (int k = 0; k < 3; ++k) {
			Frame* f0 = res->getFrame(k*3);
			Frame* f1 = res->getFrame(k*3+1);
			Frame* f2 = res->getFrame(k*3+2);
			int cf0 = 0, cf1 = 0, cf2 = 0;
			for (int i = 0; i < f0->getHeight(); ++i) {
				for (int j = 0; j < org.getWidth()*3; ++j) {


					assert(oInd < org.getSizeOfPixel());
					assert(k*3+2 < res->getSizeOfFrames());
					assert(cf0 <= res->getFrame(k*3)->getHeight()*res->getFrame(k*3)->getWidth()*3);
					assert(cf1 <= res->getFrame(k*3+1)->getHeight()*res->getFrame(k*3+1)->getWidth()*3);
					assert(cf2 <= res->getFrame(k*3+2)->getHeight()*res->getFrame(k*3+2)->getWidth()*3);

					if(cf0 < (i+1)*f0->getWidth()*3)
						pixels[k*3][cf0++] = org.getPixel()[oInd++];/*std::cout<<"pixels["<<k*3<<"]["<<cf0++<<"] = org.getPixel()["<<oInd++<<"];"<<std::endl<<std::flush;*/
					else if(cf1 < (i+1)*f1->getWidth()*3)
						pixels[k*3+1][cf1++] = org.getPixel()[oInd++];/*std::cout<<"pixels["<<k*3+1<<"]["<<cf1++<<"] = org.getPixel()["<<oInd++<<"];"<<std::endl<<std::flush;*/
					else if(cf2 < (i+1)*f2->getWidth()*3)
						pixels[k*3+2][cf2++] = org.getPixel()[oInd++];/*std::cout<<"pixels["<<k*3+2<<"]["<<cf2++<<"] = org.getPixel()["<<oInd++<<"];"<<std::endl<<std::flush;*/
					else
						std::cout<<"feels like i'm doing, nothing at all"<<std::endl<<std::flush;
				}

			}
			cf0 = 0, cf1 = 0, cf2 = 0;
		}

		for (int i = 0; i < res->getSizeOfFrames(); ++i) {
			res->getFrame(i)->setPixel(pixels[i], res->getFrame(i)->getHeight()*res->getFrame(i)->getWidth()*3);
		}

		res->setGctFlag(0);
		delete[] res->getColorTable();
		res->setColorTable(NULL,0);

		//cleanup
		delete[] lcts;
		delete[] pixels;
	}

	return res;
}

Gif* TableConverter::localToGlobal(const Gif* p_gif) { //TODO:: implement transparency?
	if (p_gif->getSizeOfFrames() <= 1)
		return NULL;

	Gif* res = new Gif(*p_gif);
	bool trans = false, bg = false;

	if(res->getGctFlag())
		bg = true;

	Point bgcol;
	bgcol.x[0] = res->getColorTable()[res->getBgColor()*3];
	bgcol.x[1] = res->getColorTable()[res->getBgColor()*3+1];
	bgcol.x[2] = res->getColorTable()[res->getBgColor()*3+2];


	//std::cout<<"BGCOL"<<std::hex<<bgcol.x[0]<<" "<<bgcol.x[1]<<" "<<bgcol.x[2]<<std::endl<<std::flush;


	std::vector<Point> points;

	for (int i = 0; i < p_gif->getSizeOfFrames(); ++i) {
		Point transp;
		trans = false;
		if(res->getFrame(i)->getTranspColorFlag() == 1){
			throw std::runtime_error("Unsupported Operation: TableConverter::localToGlobal(Gif*) does not support transparency.");
			trans = true;
				if(res->getFrame(i)->getSizeOfLCT() > 0 && res->getFrame(i)->getTranspColorIndex() <= res->getFrame(i)->getSizeOfLCT()){
				transp.x[0] = res->getFrame(i)->getLct()[res->getFrame(i)->getTranspColorIndex()*3];
				transp.x[1] = res->getFrame(i)->getLct()[res->getFrame(i)->getTranspColorIndex()*3+1];
				transp.x[2] = res->getFrame(i)->getLct()[res->getFrame(i)->getTranspColorIndex()*3+2];
			}
			else{
				transp.x[0] = res->getColorTable()[res->getFrame(i)->getTranspColorIndex()*3];
				transp.x[1] = res->getColorTable()[res->getFrame(i)->getTranspColorIndex()*3+1];
				transp.x[2] = res->getColorTable()[res->getFrame(i)->getTranspColorIndex()*3+2];
			}
		}
		for (int j = 0; j < res->getFrame(i)->getHeight() * res->getFrame(i)->getWidth()*3; j+=3) {
			//j = R, j+1 = G, j+2 = B
			Point curr;
			curr.x[0] = res->getFrame(i)->getPixel()[j];
			curr.x[1] = res->getFrame(i)->getPixel()[j+1];
			curr.x[2] = res->getFrame(i)->getPixel()[j+2];


			if((trans&& curr == transp) || (bg && curr==bgcol)){
				//std::cout<<"skipped:"<<curr.x[0]<<" "<<curr.x[1]<<" "<<curr.x[2]<<"("<<transp.x[0]<<" "<<transp.x[1]<<" "<<transp.x[2]<<")"<<std::endl<<std::flush;
				continue;
			}

			points.push_back(curr);

		}
	}


	std::vector<Point> reducedTable = medianCut(points.data(),points.size(),254);


	/*int z = 0;
		for( auto it = reducedTable.begin();
			 it != reducedTable.end(); ++it)
				std::cout<<z++<<" "<<std::hex<<(int)(*it).x[0]<<" "<<(int)(*it).x[1]<<" "<<(int)(*it).x[2]<<std::endl<<std::flush;*/


	applyColorTable(res, reducedTable);

	return res;
}

void TableConverter::applyColorTable(Gif* p_gif, std::vector<Point> p_colorTable){
	std::map<Point, int> lookup;
	int r, g, b, minEuclidDistIndex = 0;
	double euclidDist = 0, minEuclidDist = 500;

	Point newTransp = findUnusedColor(p_gif);
	bool trans = false, bg = false;

	if(p_gif->getGctFlag())
		bg = true;


	Point bgcol;
	bgcol.x[0] = p_gif->getColorTable()[p_gif->getBgColor()*3];
	bgcol.x[1] = p_gif->getColorTable()[p_gif->getBgColor()*3+1];
	bgcol.x[2] = p_gif->getColorTable()[p_gif->getBgColor()*3+2];


	for (int i = 0; i < p_gif->getSizeOfFrames(); ++i) {
		Point transp;
		trans = false;
		if(p_gif->getFrame(i)->getTranspColorFlag() == 1){
			trans = true;
			if(p_gif->getFrame(i)->getSizeOfLCT() > 0 && p_gif->getFrame(i)->getTranspColorIndex() <= p_gif->getFrame(i)->getSizeOfLCT()){
				transp.x[0] = p_gif->getFrame(i)->getLct()[p_gif->getFrame(i)->getTranspColorIndex()*3];
				transp.x[1] = p_gif->getFrame(i)->getLct()[p_gif->getFrame(i)->getTranspColorIndex()*3+1];
				transp.x[2] = p_gif->getFrame(i)->getLct()[p_gif->getFrame(i)->getTranspColorIndex()*3+2];
			}
			else{
				transp.x[0] = p_gif->getColorTable()[p_gif->getFrame(i)->getTranspColorIndex()*3];
				transp.x[1] = p_gif->getColorTable()[p_gif->getFrame(i)->getTranspColorIndex()*3+1];
				transp.x[2] = p_gif->getColorTable()[p_gif->getFrame(i)->getTranspColorIndex()*3+2];
			}
		}
		for (int j = 0; j < p_gif->getFrame(i)->getHeight() * p_gif->getFrame(i)->getWidth()*3; j+=3) {

			//find best matching color
			euclidDist = 0;
			minEuclidDist = 500;
			minEuclidDistIndex = 0;

			Point curr;
			curr.x[0] = p_gif->getFrame(i)->getPixel()[j];
			curr.x[1] = p_gif->getFrame(i)->getPixel()[j+1];
			curr.x[2] = p_gif->getFrame(i)->getPixel()[j+2];

			if(bg && curr == bgcol)
				continue;

			if(trans && curr == transp){
				p_gif->getFrame(i)->getPixel()[j] = newTransp.x[0];
				p_gif->getFrame(i)->getPixel()[j+1] = newTransp.x[1];
				p_gif->getFrame(i)->getPixel()[j+2] = newTransp.x[2];

				//std::cout<<"replaced:"<<curr.x[0]<<" "<<curr.x[1]<<" "<<curr.x[2]<<" with ("<<newTransp.x[0]<<" "<<newTransp.x[1]<<" "<<newTransp.x[2]<<")"<<std::endl<<std::flush;
				continue;
			}

			if(lookup.count(curr) == 0){ //not in lookup table
				for (int k = 0; k < (int)p_colorTable.size(); ++k) {
					r = curr.x[0] - p_colorTable[k].x[0];
					g = curr.x[1] - p_colorTable[k].x[1];
					b = curr.x[2] - p_colorTable[k].x[2];
					euclidDist = sqrt(r*r + g*g + b*b);

					if(euclidDist < minEuclidDist){
						minEuclidDist = euclidDist;
						minEuclidDistIndex = k;
					}
				}
				//lookup[curr] = minEuclidDistIndex;
			}
			else{
				minEuclidDistIndex = lookup[curr];
			}

			//apply color
			p_gif->getFrame(i)->getPixel()[j] = p_colorTable[minEuclidDistIndex].x[0];
			p_gif->getFrame(i)->getPixel()[j+1] = p_colorTable[minEuclidDistIndex].x[1];
			p_gif->getFrame(i)->getPixel()[j+2] = p_colorTable[minEuclidDistIndex].x[2];
		}
	}


	p_colorTable.push_back(newTransp);

	p_colorTable.insert(p_colorTable.begin() + p_gif->getBgColor(), bgcol);


	char* newTable = createTableArray(p_colorTable);


	/*int z = 0;
		for( auto it = p_colorTable.begin();
			 it != p_colorTable.end(); ++it)
				std::cout<<z++<<" "<<std::hex<<(int)(*it).x[0]<<" "<<(int)(*it).x[1]<<" "<<(int)(*it).x[2]<<std::endl<<std::flush;*/


	insertGlobalTable(p_gif, newTable, p_colorTable.size(), 255);
}

void TableConverter::insertGlobalTable(Gif* p_gif, char* p_newTable, int p_sizeNewTable, int p_newTranspIndex){
	if(p_gif->getColorTable() != NULL){
		delete[] p_gif->getColorTable();
		p_gif->setColorTable(NULL,0);
	}

	p_gif->setGctFlag(1);
	p_gif->setSizeOfGCT(p_sizeNewTable);
	p_gif->setColorTable(p_newTable, p_sizeNewTable);

	for (int i = 0; i < p_gif->getSizeOfFrames(); ++i) {

		if(p_gif->getFrame(i)->getTranspColorFlag() == 1)
			p_gif->getFrame(i)->setTranspColorIndex(p_newTranspIndex);

		p_gif->getFrame(i)->setLctFlag(0);
		if(p_gif->getFrame(i)->getLct() != NULL){
			delete[] p_gif->getFrame(i)->getLct();
			p_gif->getFrame(i)->setLct(NULL,0);
		}
	}
}

char* TableConverter::createTableArray(const std::vector<Point> p_colorTable){

	char* table = new char[p_colorTable.size()*3];

	for (size_t i = 0; i < p_colorTable.size(); ++i) {
		table[i*3] = p_colorTable[i].x[0];
		table[i*3+1] = p_colorTable[i].x[1];
		table[i*3+2] = p_colorTable[i].x[2];
	}
	return table;
}

char** TableConverter::copyTableMultiple(char* p_table, int p_tableSize, int p_numberCopies){

	char** ret = NULL;
	ret = new char*[p_numberCopies];
	for (int i = 0; i < p_numberCopies; ++i) {
		ret[i] = new char[p_tableSize];
		memcpy(ret[i],p_table,p_tableSize);
	}

	return ret;
}


Point TableConverter::findUnusedColor(Gif* p_gif){
	bool end = false;
	int took = 0;
	Point unused;
	unused.x[0] = 13;
	unused.x[1] = 17;
	unused.x[2] = 15;

	while(!end){

		unused = randomizeColor(unused, took);

		for (int i = 0; i < p_gif->getSizeOfFrames(); ++i){
			int j =0;
			for (j = 0; j < p_gif->getFrame(i)->getWidth()*p_gif->getFrame(i)->getHeight()*3; j+=3) {
				Point curr;
				curr.x[0] = p_gif->getFrame(i)->getPixel()[j];
				curr.x[1] = p_gif->getFrame(i)->getPixel()[j+1];
				curr.x[2] = p_gif->getFrame(i)->getPixel()[j+2];

				if(curr == unused)
					break;
			}
			if(j < p_gif->getFrame(i)->getWidth()*p_gif->getFrame(i)->getHeight()*3)
				break;
			end = true;
		}
		took++;
	}
	//std::cout<<"randomizing took "<<took<<"tries. god dammit."<<std::endl<<std::flush;
	return unused;
}


Point TableConverter::randomizeColor(Point p_start, int seed){
	p_start.x[seed%3]++;
	p_start.x[0]%=256;
	p_start.x[1]%=256;
	p_start.x[2]%=256;

	return p_start;
}







