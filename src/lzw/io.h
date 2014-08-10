#ifndef IO_H
#define IO_H
#include "gif.h"
#include "lzw.h"
#include <fstream>
#include <iostream>
#include <vector>


/**
 * @brief
 *
 */
class IO
{
	/**
	 * @brief Decompresses a frame of the internal gif using lzw.
	 *
	 * @param p_frame - number of the frame to decompress.
	 */
	void decompress(int p_frame);
	std::string m_fileName;
	int m_gce, m_par, m_pte, m_appEx, m_commEx;
	char *m_fileContent;
	unsigned char *m_rawData;
	char* m_output;
	unsigned char* m_LCT;
	unsigned char* m_GCT;
	unsigned char* m_compData;
	Gif gif;
	LZW m_lzw;
	/**
	 * @brief Reads the screen setting and writes into the internal Gif object.
	 *
	 * @param p_pointer - Pointer to the current byte in the raw data.
	 */
	void getScreen(int& p_pointer);
	/**
	 * @brief Reads the LCT and writes it into the given Frame of the internal Gif object.
	 *
	 * @param p_pointer - Pointer to the current byte in the raw data.
	 * @param p_frame - The Frame index to write into.
	 */
	void getLCT(int &p_pointer, int p_frame);
	/**
	 * @brief Reads the GCT and writes it into the internal Gif object.
	 *
	 * @param p_pointer - Pointer to the current byte in the raw data.
	 */
	void getGCT(int &p_pointer);
	/**
	 * @brief Reads the graphics control extension and writes it into the given Frame of the internal Gif object.
	 *
	 * @param p_pointer - Pointer to the current byte in the raw data.
	 * @param p_frame - The Frame index to write into.
	 */
	void getGCE(int &p_pointer, int p_frame);
	/**
	 * @brief Reads the image discription and writes it into the given Frame of the internal Gif object.
	 *
	 * @param p_pointer - Pointer to the current byte in the raw data.
	 * @param p_frame - The Frame index to write into.
	 */
	void getIDiscr(int &p_pointer, int p_frame);
	/**
	 * @brief Reads the image data and writes it into the given Frame of the internal Gif object.
	 *
	 * @param p_pointer - Pointer to the current byte in the raw data.
	 * @param p_frame - The Frame index to write into.
	 */
	void getIData(int &p_pointer, int p_frame);
	/**
	 * @brief Reads the trailer.
	 *
	 * @param pointer - Pointer to the current byte in the raw data.
	 */
	void getTrailer(int &pointer);
	/**
	 * @brief
	 *
	 * @param p_pointer
	 * @return unsigned int
	 */
	unsigned int getNextByte(int &p_pointer);
	/**
	 * @brief
	 *
	 * @param p_fileName
	 * @param p_content
	 * @param p_size
	 */
	void getFile(char *p_fileName, char *p_content, int p_size);
	/**
	 * @brief Saves a .gif-file to the given file path.
	 *
	 *	The data is read from the given unsigned char array and has to be constructed beforehand.
	 *
	 * @param p_fileName - File path
	 * @param p_output - data to write
	 * @param p_fileSize - size of the data to write.
	 */
	void saveFile(char *p_fileName, unsigned char *p_output, int p_fileSize);
	/**
	 * @brief Reads the comment extension and writes the content to the console (cout).
	 *
	 * @param pointer - Pointer to the current byte in the raw data.
	 */
	void getCommEx(int& pointer);
	/**
	 * @brief Reads the application extension and writes the content to the console (cout).
	 *
	 * @param pointer - Pointer to the current byte in the raw data.
	 */
	void getAppEx(int &pointer);
	/**
	 * @brief Prepare a gif object to save it to a file.
	 *
	 *	This includes generating the data used in LZW encode and applying LZE encode
	 *
	 * @param p_gif - the gif to prepare
	 */
	void prepareData(Gif& p_gif);
	/**
	 * @brief Writes all frames of the given Gif into the vector of unsigned char.
	 *
	 * @param p_outputData - the vector to write the bytes into.
	 * @param p_gif - the gif to read frames from.
	 */
	void writeFrames(std::vector<unsigned char> &p_outputData, Gif& p_gif);
	/**
	 * @brief Writes the header of the given Gif into the vector of unsigned char.
	 *
	 * @param p_outputData - the vector to write the bytes into.
	 * @param p_gif - the gif to read the header from.
	 */
	void writeHeader(std::vector<unsigned char> &p_outputData, Gif& p_gif);
	/**
	 * @brief Writes the graphics control extension of the specified frame of the given gif.
	 *
	 * @param p_outputData - the vector to write the bytes into.
	 * @param p_gif - the gif.
	 * @param p_frame - the index of the frame to read the GCE from.
	 */
	void writeGCE(std::vector<unsigned char> &p_outputData, Gif& p_gif, int p_frame);
	/**
	 * @brief Writes the NETSCAPE2.0 application extension.
	 *
	 *	The NETSCAPE2.0 application extension tells the decoder to loop the gif.
	 *
	 * @param p_outputData - the vector to write the bytes into.
	 */
	void writeAppExt(std::vector<unsigned char> &p_outputData);
	/**
	 * @brief Writed the image block of a frame.
	 *
	 *	Used in writeFrames(std::vector<unsigned char>, Gif&).
	 *
	 * @param p_outputData - the vector to write the bytes into.
	 * @param p_gif - the gif.
	 * @param p_frame - the index of the frame to read the image block from.
	 */
	void writeImageBlock(std::vector<unsigned char> &p_outputData, Gif& p_gif, int p_frame);
	/**
	 * @brief Writes the gif trailer.
	 *
	 * @param p_outputData - the vector to write the bytes into.
	 */
	void writeTrailer(std::vector<unsigned char> &p_outputData);


public:
	/**
	 * @brief Prepares a frame of the given Gif to be encoded with LZW.
	 *
	 * @param p_gif - the gif-object
	 * @param p_frame - number of the frame to process
	 * @param p_mode - the mode of the preparation: 0 to keep colortable, 1 to generate a table using median-cut, 2 to generate a table by colors found
	 */
	static void generateRawData(Gif &p_gif, int p_frame, int p_mode);
	/**
	 * @brief Generates a (unsigned char) Pixel-array and inserts it into a Frame of the Gif.
	 *
	 * @param p_gif - the gif to process.
	 * @param p_frame - the index of the frame to process.
	 */
	static void generatePixel(Gif &p_gif, int p_frame);
	/**
	 * @brief Default ctor
	 *
	 */
	IO();
	/**
	 * @brief dtor
	 *
	 */
	~IO();
	/**
	 * @brief Constructor that also sets a path to a .gif-file to process later (loading/saving).
	 *
	 * @param p_filePath - path to the file.
	 */
	IO(std::string p_filePath);
	/**
	 * @brief Loads the Gif given by the filepath, set by constructor.
	 *
	 */
	void loadFile();
	/**
	 * @brief Saves a gif object to a .gif-file.
	 *
	 * @param p_gif - the gif object to encode and save.
	 */
	void saveGif(Gif &p_gif);
	/**
	 * @brief Returns a pointer to the internal Gif.
	 *
	 * @return Gif - Pointer to the Gif.
	 */
	Gif* getGif();
	/**
	 * @brief Sets the internal gif.
	 *
	 * @param gif - the gif.
	 */
	void setGif(Gif &gif);
	/**
	 * @brief Checks whether delaytimes in all frames if the Gif are zero (gif not animated)
	 *
	 * @param p_gif - The Gif to check.
	 * @return bool - true if not animated, false otherwise.
	 */
	static bool checkDelayTime(Gif &p_gif);
	/**
	 * @brief
	 *
	 * @param wert
	 * @param bit
	 * @param p_count
	 * @return int
	 */
	static int getBit(int wert, int bit, int p_count);
	/**
	 * @brief
	 *
	 * @param p_exponent
	 * @return int
	 */
	static int zweiHochX(int p_exponent);
	/**
	 * @brief
	 *
	 * @param p_binary
	 * @return char
	 */
	static char getHex(int p_binary);
	/**
	 * @brief
	 *
	 * @param p_binary
	 * @return unsigned char
	 */
	static unsigned char getBinChar(unsigned int p_binary);
};

#endif // IO_H
