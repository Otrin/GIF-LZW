/*#ifndef LZW2_H
#define LZW2_H

#include <vector>
#include "frame.h"

#define byte unsigned char

class lzw2
{
public:
	lzw2(Frame f);
	void ToIndexedColor(byte r[][], byte g[][], byte b[][]);

	class stringtable{
	public:
		stringtable();
		static const int RES_CODES = 2;
		static const short HASH_FREE = (short) 0xFFFF;
		static const short NEXT_FIRST = (short) 0xFFFF;
		static const int MAXBITS = 12;
		static const int MAXSTR = (1<<MAXBITS);
		static const short HASHSIZE = 9973;
		static const short HASHSTEP = 2039;
		std::vector<byte> m_strChr;
		std::vector<short> m_strNxt;
		std::vector<short> m_strHsh;
		short m_numStrings;
	};

};

#endif // LZW2_H
*/
