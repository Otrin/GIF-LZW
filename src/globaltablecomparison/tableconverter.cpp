#include <map>
#include <math.h>

#include "tableconverter.h"
#include "gif.h"

Gif* TableConverter::globalToLocal(const Gif* p_gif) {
	if (!p_gif->getGctFlag())
		return NULL;


	Gif* res = new Gif(*p_gif);

	if (res->getSizeOfFrames() > 1) { //easymode
		for (int i = 0; i < res->getSizeOfFrames(); ++i) {
				Frame* frame = res->getFrame(i);
				frame->setLctFlag(1);
				frame->setLct(res->getColorTable(), res->getSizeOfGCT()); //have table only exist once (shallow copies, hope this works)
			}
	}
	else{ //hardmode (split shit, static one frame gifs)
		//TODO
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

	std::vector<Point> reducedTable = medianCut(points.data(),points.size(),256);


//	for (auto p : reducedTable) {
//		qDebug()<<"("<<p.x[0]<<","<<p.x[1]<<","<<p.x[2]<<")"<<",";
//	}

	applyColorTable(res, reducedTable);

	//TODO: change gct,lct flags around and write new table into res

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
}
