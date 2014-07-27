/*#include "lzw2.h"


lzw2::lzw2(Frame f){
	width_ = (short) image.getWidth(null);
	height_ = (short) image.getHeight(null);
	int values[] = new int[width_*height_];
	PixelGrabber grabber = new PixelGrabber(image, 0, 0, width_, height_, values, 0, width_);
	try {
	  if(grabber.grabPixels()!=true) {
		throw new AWTException("Grabber returned false: "+grabber.status()); //$NON-NLS-1$
	  }
	} catch(InterruptedException ex) {
	  ex.printStackTrace();
	}
	byte r[][] = new byte[width_][height_];
	byte g[][] = new byte[width_][height_];
	byte b[][] = new byte[width_][height_];
	int index = 0;
	for(int y = 0; y<height_; ++y) {
	  for(int x = 0; x<width_; ++x) {
		r[x][y] = (byte) ((values[index]>>16)&0xFF);
		g[x][y] = (byte) ((values[index]>>8)&0xFF);
		b[x][y] = (byte) ((values[index])&0xFF);
		++index;
	  }
	}
	ToIndexedColor(r, g, b);
}


void ToIndexedColor(byte r[][], byte g[][], byte b[][]){
  pixels_ = new byte[width_*height_];
  colors_ = new byte[256*3];
  int colornum = 0;
  for(int x = 0; x<width_; ++x) {
	for(int y = 0; y<height_; ++y) {
	  int search;
	  for(search = 0; search<colornum; ++search) {
		if((colors_[search*3]==r[x][y])&&(colors_[search*3+1]==g[x][y])&&(colors_[search*3+2]==b[x][y])) {
		  break;
		}
	  }
	  if(search>255) {
		throw new AWTException("Too many colors."); //$NON-NLS-1$
	  }
	  pixels_[y*width_+x] = (byte) search;
	  if(search==colornum) {
		colors_[search*3] = r[x][y];
		colors_[search*3+1] = g[x][y];
		colors_[search*3+2] = b[x][y];
		++colornum;
	  }
	}
  }
  numColors_ = 1<<BitUtils.BitsNeeded(colornum);
  byte copy[] = new byte[numColors_*3];
  System.arraycopy(colors_, 0, copy, 0, numColors_*3);
  colors_ = copy;
}


class LZWStringTable {
private final static int RES_CODES = 2;
private final static short HASH_FREE = (short) 0xFFFF;
private final static short NEXT_FIRST = (short) 0xFFFF;
private final static int MAXBITS = 12;
private final static int MAXSTR = (1<<MAXBITS);
private final static short HASHSIZE = 9973;
private final static short HASHSTEP = 2039;
byte strChr_[];
short strNxt_[];
short strHsh_[];
short numStrings_;

LZWStringTable() {
  strChr_ = new byte[MAXSTR];
  strNxt_ = new short[MAXSTR];
  strHsh_ = new short[HASHSIZE];
}

int AddCharString(short index, byte b) {
  int hshidx;
  if(numStrings_>=MAXSTR) {
	return 0xFFFF;
  }
  hshidx = Hash(index, b);
  while(strHsh_[hshidx]!=HASH_FREE) {
	hshidx = (hshidx+HASHSTEP)%HASHSIZE;
  }
  strHsh_[hshidx] = numStrings_;
  strChr_[numStrings_] = b;
  strNxt_[numStrings_] = (index!=HASH_FREE) ? index : NEXT_FIRST;
  return numStrings_++;
}

short FindCharString(short index, byte b) {
  int hshidx, nxtidx;
  if(index==HASH_FREE) {
	return b;
  }
  hshidx = Hash(index, b);
  while((nxtidx = strHsh_[hshidx])!=HASH_FREE) {
	if((strNxt_[nxtidx]==index)&&(strChr_[nxtidx]==b)) {
	  return(short) nxtidx;
	}
	hshidx = (hshidx+HASHSTEP)%HASHSIZE;
  }
  return(short) 0xFFFF;
}

void ClearTable(int codesize) {
  numStrings_ = 0;
  for(int q = 0; q<HASHSIZE; q++) {
	strHsh_[q] = HASH_FREE;
  }
  int w = (1<<codesize)+RES_CODES;
  for(int q = 0; q<w; q++) {
	AddCharString((short) 0xFFFF, (byte) q);
  }
}

static int Hash(short index, byte lastbyte) {
  return(((short) (lastbyte<<8)^index)&0xFFFF)%HASHSIZE;
}

}

class LZWCompressor {
static void LZWCompress(OutputStream output, int codesize, byte toCompress[]) throws IOException {
  byte c;
  short index;
  int clearcode, endofinfo, numbits, limit;
  short prefix = (short) 0xFFFF;
  BitFile bitFile = new BitFile(output);
  LZWStringTable strings = new LZWStringTable();
  clearcode = 1<<codesize;
  endofinfo = clearcode+1;
  numbits = codesize+1;
  limit = (1<<numbits)-1;
  strings.ClearTable(codesize);
  bitFile.WriteBits(clearcode, numbits);
  for(int loop = 0; loop<toCompress.length; ++loop) {
	c = toCompress[loop];
	if((index = strings.FindCharString(prefix, c))!=-1) {
	  prefix = index;
	} else {
	  bitFile.WriteBits(prefix, numbits);
	  if(strings.AddCharString(prefix, c)>limit) {
		if(++numbits>12) {
		  bitFile.WriteBits(clearcode, numbits-1);
		  strings.ClearTable(codesize);
		  numbits = codesize+1;
		}
		limit = (1<<numbits)-1;
	  }
	  prefix = (short) (c&0xFF);
	}
  }
  if(prefix!=-1) {
	bitFile.WriteBits(prefix, numbits);
  }
  bitFile.WriteBits(endofinfo, numbits);
  bitFile.Flush();
}

}
*/
