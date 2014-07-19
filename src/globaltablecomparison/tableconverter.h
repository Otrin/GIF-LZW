#ifndef TABLECONVERTER_H
#define TABLECONVERTER_H

#include "gif.h"
#include "median_cut.h"

/**
 * @brief This class converts colortables of gif objects from local to global and vice versa.
 *
 */
class TableConverter
{
public:
	static Gif* globalToLocal(const Gif* p_gif);
	static Gif* localToGlobal(const Gif* p_gif);
private:
	static void applyColorTable(Gif* p_gif, const std::vector<Point> p_colorTable);
	static void insertGlobalTable(Gif* p_gif, char* p_newTable, int p_sizeNewTable, bool removeLocals);
	static char* createTableArray(const std::vector<Point> p_colorTable, int &p_outputSize);
	static char** copyTableMultiple(char* p_table, int p_tableSize, int p_numberCopies);
};

#endif // TABLECONVERTER_H
