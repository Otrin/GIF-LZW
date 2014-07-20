#include <map>
#include <math.h>
#include <cstring>

#include<iostream>
#include<fstream>
#include<sstream>

#include "tableconverter.h"
#include "gif.h"
#include "frame.h"

Gif* TableConverter::globalToLocal(const Gif* p_gif) {
	if (!p_gif->getGctFlag())
		return NULL;



	Gif* res = new Gif(*p_gif);

	if (res->getSizeOfFrames() > 1) { //easymode
		for (int i = 0; i < res->getSizeOfFrames(); ++i) {
				Frame* frame = res->getFrame(i);
				frame->setLctFlag(1);
				frame->setLct(res->getColorTable(), res->getSizeOfGCT()); //have table only exist once (shallow copies, hope this works) ///CHANGE!!!
			}
	}
	else{ //hardmode (split shit, static one frame gifs)


		const int numSegments = 3; //doesn't really work with other formats (doesn't even work with this one :p)

		Frame org = *(res->getFrame(0)); //disposal hint: current destructor of frame is unsufficient, needs to delete all internal shizzlewhizzle

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

		char** lcts = copyTableMultiple(res->getColorTable(),res->getSizeOfGCT(),numSegments*numSegments);

		char** pixels = new char*[res->getSizeOfFrames()];

		for (int i = 0; i < res->getSizeOfFrames(); ++i) {

			if((i+1) % numSegments == 0){ //frames with extra width

				res->getFrame(i)->setWidth(blockSizeW+marginW);
				res->getFrame(i)->setHeight(blockSizeH);

			}
			if(i >= (numSegments*(numSegments-1))){ //frames with extra height
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

			//for some reason they are nonNULL even though the ctor sets them to null
			res->getFrame(i)->setCodeTable(NULL,0);
			res->getFrame(i)->setUserInputFlag(0);
			res->getFrame(i)->setInterlaceFlag(org.getInterlaceFlag()); //is this gonna work?
			res->getFrame(i)->setDisposualMethod(org.getDisposualMethod());
			res->getFrame(i)->setPixel(NULL,0);
			res->getFrame(i)->setCodeTable(NULL,0);

		}	


		//test the following

		int oInd = 0;
		for (int k = 0; k < 3; ++k) {
			Frame* f0 = res->getFrame(k*3);
			Frame* f1 = res->getFrame(k*3+1);
			int cf0 = 0, cf1 = 0, cf2 = 0;
			for (int i = 0; i < f0->getHeight(); ++i) {
				for (int j = 0; j < org.getWidth()*3; ++j) {

					/*std::cout<<"k"<<k<<"i"<<i<<"j"<<j;
					std::cout<<" cf0 "<<cf0;
					std::cout<<" cf1 "<<cf1;
					std::cout<<" cf2 "<<cf2<<std::endl<<std::flush;*/
					if(cf0 < (i+1)*f0->getWidth()*3)
						pixels[k*3][cf0++] = org.getPixel()[oInd++];/*std::cout<<"pixels["<<k*3<<"]["<<cf0++<<"] = org.getPixel()["<<oInd++<<"];"<<std::endl<<std::flush;*/
					else if(cf1 < (i+1)*f1->getWidth()*3)
						pixels[k*3+1][cf1++] = org.getPixel()[oInd++];/*std::cout<<"pixels["<<k*3+1<<"]["<<cf1++<<"] = org.getPixel()["<<oInd++<<"];"<<std::endl<<std::flush;*/
					else
						pixels[k*3+2][cf2++] = org.getPixel()[oInd++];/*std::cout<<"pixels["<<k*3+2<<"]["<<cf2++<<"] = org.getPixel()["<<oInd++<<"];"<<std::endl<<std::flush;*/
				}

			}
			cf0 = 0, cf1 = 0, cf2 = 0;
		}

		for (int i = 0; i < res->getSizeOfFrames(); ++i) {
			res->getFrame(i)->setPixel(pixels[i], res->getFrame(i)->getHeight()*res->getFrame(i)->getWidth()*3);
		}

		/*ofstream out("gpx.txt");
		if(out)
		out.write(org.getPixel(),org.getSizeOfPixel());
		out.close();


		for (int i = 0; i < res->getSizeOfFrames(); ++i) {
			std::ostringstream fname;
			fname << "lpx" << i << ".txt";
			ofstream out(fname.str().c_str());
			if(out){
				out.write(res->getFrame(i)->getPixel(),res->getFrame(i)->getSizeOfPixel());
				out.close();
			}
		}*/

		res->setGctFlag(0);
		delete[] res->getColorTable();
		res->setColorTable(NULL,0);

		//cleanup
		delete[] lcts;
		delete[] pixels;
	}

	return res;
}

Gif* TableConverter::localToGlobal(const Gif* p_gif) {
	if (p_gif->getGctFlag() && p_gif->getSizeOfFrames() <= 1)
		return NULL;

	Gif* res = new Gif(*p_gif);

	std::vector<Point> points;

	for (int i = 0; i < p_gif->getSizeOfFrames(); ++i) {
		for (int j = 0; j < res->getFrame(i)->getHeight() * res->getFrame(i)->getWidth(); j+=3) {
			//j = R, j+1 = G, j+2 = B
			Point curr;
			curr.x[0] = res->getFrame(i)->getPixel()[j];
			curr.x[1] = res->getFrame(i)->getPixel()[j+1];
			curr.x[2] = res->getFrame(i)->getPixel()[j+2];
			points.push_back(curr);
		}
	}

	if(p_gif->getGctFlag()){ //add original global table as well
		for (int i = 0; i < res->getSizeOfGCT(); i+=3) {
			Point curr;
			curr.x[0] = res->getColorTable()[i];
			curr.x[1] = res->getColorTable()[i+1];
			curr.x[2] = res->getColorTable()[i+2];
			points.push_back(curr);
		}
	}

	std::vector<Point> reducedTable = medianCut(points.data(),points.size(),256);


//	for (auto p : reducedTable) {
//		qDebug()<<"("<<p.x[0]<<","<<p.x[1]<<","<<p.x[2]<<")"<<",";
//	}

	applyColorTable(res, reducedTable);

	return res;
}


void TableConverter::applyColorTable(Gif* p_gif, const std::vector<Point> p_colorTable){

	std::map<Point, int> lookup;
	int r, g, b, minEuclidDistIndex = 0;
	double euclidDist = 0, minEuclidDist = 500;

	for (int i = 0; i < p_gif->getSizeOfFrames(); ++i) {
		for (int j = 0; j < p_gif->getFrame(i)->getHeight() * p_gif->getFrame(i)->getWidth(); j+=3) {

			//find best matching color
			euclidDist = 0;
			minEuclidDist = 500;
			minEuclidDistIndex = 0;

			Point curr;
			curr.x[0] = p_gif->getFrame(i)->getPixel()[j];
			curr.x[1] = p_gif->getFrame(i)->getPixel()[j+1];
			curr.x[2] = p_gif->getFrame(i)->getPixel()[j+2];

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
				lookup[curr] = minEuclidDistIndex;
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

	int size = 0;
	char* newTable = createTableArray(p_colorTable,size);
	insertGlobalTable(p_gif, newTable, size, true);

}

void TableConverter::insertGlobalTable(Gif* p_gif, char* p_newTable, int p_sizeNewTable, bool removeLocals){

	if(p_gif->getColorTable() != NULL){
		delete[] p_gif->getColorTable();
		p_gif->setColorTable(NULL,0);
	}

	p_gif->setGctFlag(1);
	p_gif->setSizeOfGCT(p_sizeNewTable);
	p_gif->setColorTable(p_newTable, p_sizeNewTable);

	if(removeLocals)
		for (int i = 0; i < p_gif->getSizeOfFrames(); ++i) {
			p_gif->getFrame(i)->setLctFlag(0);

			if(p_gif->getFrame(i)->getLct() != NULL){
				delete[] p_gif->getFrame(i)->getLct();
				p_gif->getFrame(i)->setLct(NULL,0);
			}
		}
}

char* TableConverter::createTableArray(const std::vector<Point> p_colorTable, int& p_outputSize){

	p_outputSize = p_colorTable.size()*3;
	char* table = new char[p_outputSize];

	for (int i = 0; i < p_outputSize; i+=3) {
		table[i] = p_colorTable[(int)(i/3)].x[0];
		table[i+1] = p_colorTable[(int)(i/3)].x[1];
		table[i+2] = p_colorTable[(int)(i/3)].x[2];
	}
	return table;
}

char** TableConverter::copyTableMultiple(char* p_table, int p_tableSize, int p_numberCopies){

	char** ret;
	ret = new char*[p_numberCopies];

	for (int i = 0; i < p_numberCopies; ++i) {
		ret[i] = new char[p_tableSize];
		memcpy(ret[i],p_table,p_tableSize);
		//std::copy(std::begin(p_table), std::end(p_table), std::begin(ret[i]));
	}

	return ret;
}
