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

    switch (m_mode) {
	case Global_to_Local:{
        res = TableConverter::globalToLocal(m_gif);
		clock_t begin = clock();

		//use lzw on res (output)

		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

		std::cout << "elapsed time:"<<elapsed_secs<<std::endl;

	   } break;
	case Local_to_Global:{
        res = TableConverter::localToGlobal(m_gif);
		clock_t begin = clock();

		//use lzw on res (output)

		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

		std::cout << "elapsed time:"<<elapsed_secs<<std::endl;
	   } break;
    default:
        emit error(QString("Invalid mode. (") + m_mode + ")");
        break;
    }

    if(res == NULL)
        emit error("Conversion failed.");


    emit result(res);
    emit finished();

}
