#include <ctime>

#include "tableconversionworker.h"
#include "tableconverter.h"

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


	stat->mode = m_mode;

    switch (m_mode) {
	case Global_to_Local:{

		clock_t cBegin = clock();
		res = TableConverter::globalToLocal(m_gif);
		clock_t cEnd = clock();
		double cElapsedSecs = double(cEnd - cBegin)/* / CLOCKS_PER_SEC*/;
		stat->conversionTime = cElapsedSecs;

		clock_t lBegin = clock();
		//use lzw on res (output)
		clock_t lEnd = clock();
		double lElapsedSecs = double(lEnd - lBegin)/* / CLOCKS_PER_SEC*/;
		stat->newLZWTime = lElapsedSecs;
		//std::cout << "elapsed time:"<<elapsed_secs<<std::endl;

		//TODO:save original for comparison


	   } break;
	case Local_to_Global:{

		clock_t cBegin = clock();
		res = TableConverter::localToGlobal(m_gif);
		clock_t cEnd = clock();
		double cElapsedSecs = double(cEnd - cBegin)/* / CLOCKS_PER_SEC*/;
		stat->conversionTime = cElapsedSecs;

		clock_t lBegin = clock();
		//use lzw on res (output)
		clock_t lEnd = clock();
		double lElapsedSecs = double(lEnd - lBegin)/* / CLOCKS_PER_SEC*/;
		stat->newLZWTime = lElapsedSecs;
		//std::cout << "elapsed time:"<<elapsed_secs<<std::endl;

		//TODO:save original for comparison

	   } break;
    default:
        emit error(QString("Invalid mode. (") + m_mode + ")");
        break;
    }

    if(res == NULL)
        emit error("Conversion failed.");

	emit statisticsOut(stat);

    emit conversionDone(res);
    emit finished();

}
