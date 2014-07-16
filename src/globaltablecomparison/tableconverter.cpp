#include <map>
#include <QDebug>

#include "tableconverter.h"
#include "gif.h"
#include "median_cut.h"

Gif* TableConverter::globalToLocal(Gif* p_gif) {
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
	else{ //hardmode (split shit)
		//TODO
	}


	return res;
}

Gif* TableConverter::localToGlobal(Gif* p_gif) {
	if (p_gif->getGctFlag() && p_gif->getSizeOfFrames() == 1)
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

	std::list<Point> reducedTable = medianCut(points.data(),points.size(),256);


	for (auto p : reducedTable) {
		qDebug()<<"("<<p.x[0]<<","<<p.x[1]<<","<<p.x[2]<<")"<<",";
	}

	//table done, now apply it to frames

	return res;
}

