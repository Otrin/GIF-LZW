#include <ctime>

#include "tableconversionworker.h"
#include "tableconverter.h"

TableConversionWorker::TableConversionWorker(Mode p_mode, Gif* p_gif){
    m_mode = p_mode;
    m_gif = p_gif;
}

TableConversionWorker::~TableConversionWorker(){
//params are observed only.
}

void TableConversionWorker::process(){

    Gif* res = NULL;
	ConversionStatistics* stat = new ConversionStatistics;


    switch (m_mode) {
	case Global_to_Local:{

		clock_t cBegin = clock();
		 res = TableConverter::globalToLocal(m_gif);
		clock_t cEnd = clock();
		double cElapsedSecs = double(cEnd - cBegin) / CLOCKS_PER_SEC;
		stat->conversionTime = cElapsedSecs;

		clock_t lBegin = clock();
		//use lzw on res (output)
		clock_t lEnd = clock();
		double lElapsedSecs = double(lEnd - lBegin) / CLOCKS_PER_SEC;
		stat->newLZWTime = lElapsedSecs;
		//std::cout << "elapsed time:"<<elapsed_secs<<std::endl;

		//TODO:save original for comparison


	   } break;
	case Local_to_Global:{

		clock_t cBegin = clock();
        res = TableConverter::localToGlobal(m_gif);
		clock_t cEnd = clock();
		double cElapsedSecs = double(cEnd - cBegin) / CLOCKS_PER_SEC;
		stat->conversionTime = cElapsedSecs;

		clock_t lBegin = clock();
		//use lzw on res (output)
		clock_t lEnd = clock();
		double lElapsedSecs = double(lEnd - lBegin) / CLOCKS_PER_SEC;
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
