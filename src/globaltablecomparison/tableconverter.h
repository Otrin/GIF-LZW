#ifndef TABLECONVERTER_H
#define TABLECONVERTER_H

#include "gif.h"
#include "median_cut.h"

#include <set>
#include <map>



/**
 * @brief This class converts colortables of gif objects from local to global and vice versa.
 *
 */
class TableConverter
{
public:
	/**
	 * @brief convert gif objects with global color table to local color tables.
	 *
	 * This static method converts a gif with global color table to a gif with local color tables in the frames.
	 * If the input gif has one frame only, the picture is divided into nine frames.
	 *
	 * @param p_gif Gif to convert (will not be changed).
	 * @return Gif The new gif with local color tables.
	 */
	static Gif* globalToLocal(const Gif *p_gif);
	/**
	 * @brief convert gif objects with local color tables to global color table.
	 *
	 * This static method converts a gif with local color tables in the frames to a gif with a global color table.
	 * If there are more than 256 colors in the local color tables, the colors are quantized using median cut.
	 *
	 * @param p_gif Gif to convert (will not be changed).
	 * @return Gif The new gif with a global color table.
	 */
	static Gif* localToGlobal(Gif *p_gif);
private:
	static void applyColorTable(Gif* p_gif, std::vector<Point> p_colorTable, std::set<Point> p_allTransies, std::map<int,Point> p_transpPerFrame);
	static void insertGlobalTable(Gif* p_gif, char* p_newTable, int p_sizeNewTable, int p_newTranspIndex);
	static char* createTableArray(const std::vector<Point> p_colorTable);
	static char** copyTableMultiple(char* p_table, int p_tableSize, int p_numberCopies);
	static Point findUnusedColor(Gif* p_gif);
	static Point randomizeColor(Point p_start, int seed);
};

#endif // TABLECONVERTER_H
