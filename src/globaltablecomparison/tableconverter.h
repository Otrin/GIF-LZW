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
	/**
	 * @brief Copies a Point vector into a explicitly allocated array.
	 *
	 * @param p_colorTable - vector to copy.
	 * @return unsigned char - a pointer to the resulting array.
	 */
	static unsigned char *createTableArray(const std::vector<Point> &p_colorTable);
private:

	/**
	 * @brief This method applies a colortable to a given gif.
	 *
	 *	This is done by calculating the euclidean distance of colors in the picture and colors in the color table.
	 *	Colors in the picture are then changed to the best match from the color table. (euclidean distance minimal)
	 *
	 * @param p_gif - gif to apply the color table to.
	 * @param p_colorTable - the color table to apply.
	 * @param p_transpPerFrame -  map that contains the transparencies of each frame.
	 * @param bgEqual - whether the bgColor is the same as the transparencies. Implies that all transparencies are the same, also.
	 */
	static void applyColorTable(Gif* p_gif, std::vector<Point> p_colorTable, std::map<int,Point> p_transpPerFrame, bool bgEqual);

	/**
	 * @brief Inserts a given Global color table into a gif.
	 *
	 * @param p_gif - the gif to insert the color table.
	 * @param p_newTable - the table to insert.
	 * @param p_sizeNewTable - the size of the table.
	 * @param p_newTranspIndex - the index in which the transparency color can be found.
	 */
	static void insertGlobalTable(Gif* p_gif, unsigned char *p_newTable, int p_sizeNewTable, int p_newTranspIndex);
	/**
	 * @brief Put multiple copies of an unsigned char array into a 2d array.
	 *
	 * @param p_table - array to copy.
	 * @param p_tableSize - array of the table.
	 * @param p_numberCopies - number of copies.
	 * @return unsigned char - pointer to the 2d array.
	 */
	static unsigned char **copyTableMultiple(unsigned char *p_table, int p_tableSize, int p_numberCopies);
	/**
	 * @brief Finds a color that is not used in a gif.
	 *
	 * @param p_gif - gif to find the unused color in.
	 * @return Point - the unused color, wrapped in Point.
	 */
	static Point findUnusedColor(Gif* p_gif);
	static Point randomizeColor(Point p_start, int seed);
};

#endif // TABLECONVERTER_H
