#ifndef GIF2_H
#define GIF2_H

#include <vector>
#include "picture.h"

#define byte unsigned char


struct header{
	byte magicnumber[3];
	byte version[3];
	byte width[2];
	byte height[2];
	byte packed;
	byte bgColIndex;
	byte pixelRatio;
};

struct imageblock{

};

struct graphicContrExt{
	byte extIntro = (byte)0x21;
	byte gcLabel = (byte)0xf9;
	byte blockSize = (byte)0x04;
	byte packed;
	byte delaytime[2];
	byte transColIndex;
	byte blockTerm = (byte)0x00;
};

struct commentExt{
	byte extIntro = (byte)0x21;
	byte commLabel = (byte)0xfc;
	byte blockSize;
	std::vector<byte> data;
	byte blockTerm = (byte)0x00;
};

struct plainTextExt{
	byte extIntro = (byte)0x21;
	byte plainTextLabel = (byte)0x01;
};

struct appExt{
};

class gif2 : public Picture
{
public:
	gif2();
};

#endif // GIF2_H
