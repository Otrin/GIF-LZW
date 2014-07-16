#ifndef TABLECONVERTER_H
#define TABLECONVERTER_H

#include "gif.h"

/**
 * @brief This class converts colortables of gif objects from local to global and vice versa.
 *
 */
class TableConverter
{
public:
	static Gif* globalToLocal(Gif* p_gif);
	static Gif* localToGlobal(Gif *p_gif);
};

#endif // TABLECONVERTER_H
