#include <ctime>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <direct.h>

#include "tableconversionworker.h"
#include "tableconverter.h"
#include "io.h"


TableConversionWorker::TableConversionWorker(Gif* p_gif){
    m_gif = p_gif;
}

TableConversionWorker::~TableConversionWorker(){
//params are observed only.
}

void TableConversionWorker::process(){

    Gif* res = NULL;
	ConversionStatistics* stat = new ConversionStatistics;

	bool lct = false;
	for (int i = 0; i < m_gif->getSizeOfFrames(); ++i) {
		if(m_gif->getFrame(i)->getLctFlag() == 1){
			lct = true;
			break;
		}
	}
	m_mode = lct?Mode::Local_to_Global:Mode::Global_to_Local;




	std::stringstream oss, lss;
	oss << "original" << clock() << ".gif";
	std::string orgFileName = oss.str();

	lss << ".\\generated\\converted" << clock() << ".gif";
	std::string lzwFileName = lss.str();


	IO orgIOFile(orgFileName);
	IO lzwIOFile(lzwFileName);

	#if defined(_WIN32)
	_mkdir("generated");
	#else
	mkdir(strPath.c_str(), 0777);
	#endif

	stat->mode = m_mode;

	emit modeOut(&m_mode);

    switch (m_mode) {
	case Global_to_Local:{

		clock_t cBegin = clock();
		res = TableConverter::globalToLocal(m_gif);
		clock_t cEnd = clock();
		double cElapsedSecs = double(cEnd - cBegin)/* / CLOCKS_PER_SEC*/;
		stat->conversionTime = cElapsedSecs;

		emit conversionDone(res);

		clock_t lBegin = clock();
		lzwIOFile.saveGif(*res);
		clock_t lEnd = clock();
		double lElapsedSecs = double(lEnd - lBegin)/* / CLOCKS_PER_SEC*/;
		stat->newLZWTime = lElapsedSecs;

		clock_t oBegin = clock();
		orgIOFile.saveGif(*m_gif);
		clock_t oEnd = clock();
		double oElapsedSecs = double(oEnd - oBegin)/* / CLOCKS_PER_SEC*/;
		stat->orgLZWTime = oElapsedSecs;

		stat->newSize = getFilesize(lzwFileName);
		stat->orgSize = getFilesize(orgFileName);

		remove(orgFileName.c_str());

	   } break;
	case Local_to_Global:{

		clock_t cBegin = clock();
		res = TableConverter::localToGlobal(m_gif);
		clock_t cEnd = clock();
		double cElapsedSecs = double(cEnd - cBegin)/* / CLOCKS_PER_SEC*/;
		stat->conversionTime = cElapsedSecs;

		emit conversionDone(res);

		clock_t lBegin = clock();
		lzwIOFile.saveGif(*res);
		clock_t lEnd = clock();
		double lElapsedSecs = double(lEnd - lBegin)/* / CLOCKS_PER_SEC*/;
		stat->newLZWTime = lElapsedSecs;

		clock_t oBegin = clock();
		orgIOFile.saveGif(*m_gif);
		clock_t oEnd = clock();
		double oElapsedSecs = double(oEnd - oBegin)/* / CLOCKS_PER_SEC*/;
		stat->orgLZWTime = oElapsedSecs;

		stat->newSize = getFilesize(lzwFileName);
		stat->orgSize = getFilesize(orgFileName);

		remove(orgFileName.c_str());

	   } break;
    default:
        emit error(QString("Invalid mode. (") + m_mode + ")");
        break;
    }

    if(res == NULL)
        emit error("Conversion failed.");

	emit statisticsOut(stat);
    emit finished();

}


size_t TableConversionWorker::getFilesize(const std::string& filename) {
	struct stat st;
	if(stat(filename.c_str(), &st) != 0) {
		return 0;
	}
	return st.st_size;
}
