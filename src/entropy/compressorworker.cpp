#include "compressorworker.h"
#include <iostream>
#include <stdio.h>
#include <QObject>
#include <sstream>
#include "compressor.h"
#include "lzw.h"
#include "huffman.h"
#include "runlengthencoding.h"

using namespace std;

CompressorWorker::CompressorWorker(){
    m_rawData = NULL;
    m_compData = NULL;
    m_codeTable = NULL;
    m_sizeOfRawData = 0;
    m_sizeOfCompData = 0;
    m_sizeOfCodeTable = 0;
    m_compressorType = HUFFMAN;
}

CompressorWorker::~CompressorWorker(){
    delete[] m_rawData;
    delete[] m_compData;
    delete[] m_codeTable;
}

void CompressorWorker::process(){
    Compressor *compressor;
    stringstream output;
    long time, s, ms, mcs;
    if(m_compressorType == lZW){
        compressor = new LZW;
		m_compData = compressor->encode(m_rawData, m_sizeOfRawData, 0);
        m_codeTable = compressor->getCodeTable();
        m_sizeOfCodeTable = compressor->getSizeOfCodeTable();
        m_sizeOfCompData = compressor->getSizeOfCompData();

		//cout<<"sizeCodet "<<m_sizeOfCodeTable<<" sizecompdata "

        output << (m_currLang=="de"?"\nKomprimieren\n\n":m_currLang=="en"?"\nCompress\n\n":"\nCompress\n\n");
        output << (m_currLang=="de"?"Kompressionsrate: ":m_currLang=="en"?"Compressionrate: ":"Compressionrate: ") << ((double)m_sizeOfRawData/(double)m_sizeOfCompData);
        time = compressor->getTimeAgo();
		mcs = 0;
//      time /=1000;
//      mcs = time % 1000;
//      time /= 1000;
		ms = time%1000;
        time /= 1000;
        s = time%1000;
        output << (m_currLang=="de"?"\nAusführungszeit: ":m_currLang=="en"?"\nExecutiontime: ": "\nExecutiontime: ")<< s << "s " << ms << "ms " << mcs << "µs";

        output << "\n\n===========================";

		m_rawData = compressor->decode(m_compData, m_sizeOfCompData, m_codeTable, m_sizeOfCodeTable, m_sizeOfRawData/3);
        output << (m_currLang=="de"?"\nDekomprimieren\n\n":m_currLang=="en"?"\nUncompress\n\n":"\nUncompress\n\n");
        output << (m_currLang=="de"?"Kompressionsrate: ":m_currLang=="en"?"Compressionrate: ":"Compressionrate: ") << ((double)m_sizeOfRawData/(double)m_sizeOfCompData);
        time = compressor->getTimeAgo();
		mcs = 0;
//      time /=1000;
//      mcs = time % 1000;
//      time /= 1000;
        ms = time%1000;
        time /= 1000;
        s = time%1000;
        output << (m_currLang=="de"?"\nAusführungszeit: ":m_currLang=="en"?"\nExecutiontime: ": "\nExecutiontime: ")<< s << "s " << ms << "ms " << mcs << "µs";
        m_information = QString::fromStdString(output.str());
	}else if(m_compressorType == HUFFMAN){
		compressor = new Huffman();
		m_compData = compressor->encode(m_rawData, m_sizeOfRawData, 0);
        m_codeTable = compressor->getCodeTable();
        m_sizeOfCodeTable = compressor->getSizeOfCodeTable();
        m_sizeOfCompData = compressor->getSizeOfCompData();

        output << (m_currLang=="de"?"\nKomprimieren\n\n":m_currLang=="en"?"\nCompress\n\n":"\nCompress\n\n");
        output << (m_currLang=="de"?"Kompressionsrate: ":m_currLang=="en"?"Compressionrate: ":"Compressionrate: ") << ((double)m_sizeOfRawData*8/(double)m_sizeOfCompData);
        time = compressor->getTimeAgo();
        time /=1000;
        mcs = time % 1000;
        time /= 1000;
        ms = time%1000;
        time /= 1000;
        s = time%1000;
        output << (m_currLang=="de"?"\nAusführungszeit: ":m_currLang=="en"?"\nExecutiontime: ": "\nExecutiontime: ")<< s << "s " << ms << "ms " << mcs << "µs";

        output << "\n\n===========================";

		m_rawData = compressor->decode(m_compData, m_sizeOfCompData, m_codeTable, m_sizeOfCodeTable, 0);
        output << (m_currLang=="de"?"\nDekomprimieren\n\n":m_currLang=="en"?"\nUncompress\n\n":"\nUncompress\n\n");
        output << (m_currLang=="de"?"Kompressionsrate: ":m_currLang=="en"?"Compressionrate: ":"Compressionrate: ") << ((double)m_sizeOfRawData*8/(double)m_sizeOfCompData);
        time = compressor->getTimeAgo();
        time /=1000;
        mcs = time % 1000;
        time /= 1000;
        ms = time%1000;
        time /= 1000;
        s = time%1000;
        output << (m_currLang=="de"?"\nAusführungszeit: ":m_currLang=="en"?"\nExecutiontime: ": "\nExecutiontime: ")<< s << "s " << ms << "ms " << mcs << "µs";
        m_information = QString::fromStdString(output.str());
    }else if(m_compressorType == RunlengthEncoding){
        compressor = new RunLengthEncoding();
		m_compData = compressor->encode(m_rawData, m_sizeOfRawData, 0);
        m_codeTable = compressor->getCodeTable();
        m_sizeOfCodeTable = compressor->getSizeOfCodeTable();
        m_sizeOfCompData = compressor->getSizeOfCompData();

        output << (m_currLang=="de"?"\nKomprimieren\n\n":m_currLang=="en"?"\nCompress\n\n":"\nCompress\n\n");
        output << (m_currLang=="de"?"Kompressionsrate: ":m_currLang=="en"?"Compressionrate: ":"Compressionrate: ") << ((double)m_sizeOfRawData/(double)m_sizeOfCompData);
        time = compressor->getTimeAgo();
        time /=1000;
        mcs = time % 1000;
        time /= 1000;
        ms = time%1000;
        time /= 1000;
        s = time%1000;
        output << (m_currLang=="de"?"\nAusführungszeit: ":m_currLang=="en"?"\nExecutiontime: ": "\nExecutiontime: ")<< s << "s " << ms << "ms " << mcs << "µs";

        output << "\n\n===========================";

		m_rawData = compressor->decode(m_compData, m_sizeOfCompData, m_codeTable, m_sizeOfCodeTable, 0);
        output << (m_currLang=="de"?"\nDekomprimieren\n\n":m_currLang=="en"?"\nUncompress\n\n":"\nUncompress\n\n");
        output << (m_currLang=="de"?"Kompressionsrate: ":m_currLang=="en"?"Compressionrate: ":"Compressionrate: ") << ((double)m_sizeOfRawData/(double)m_sizeOfCompData);
        time = compressor->getTimeAgo();
        time /=1000;
        mcs = time % 1000;
        time /= 1000;
        ms = time%1000;
        time /= 1000;
        s = time%1000;
        output << (m_currLang=="de"?"\nAusführungszeit: ":m_currLang=="en"?"\nExecutiontime: ": "\nExecutiontime: ")<< s << "s " << ms << "ms " << mcs << "µs";
        m_information = QString::fromStdString(output.str());
    }
    QString type;
    switch (m_compressorType) {
    case lZW:
        type = QString::fromStdString("LZW");
        break;
    case HUFFMAN:
        type = QString::fromStdString("HUF");
        break;
    case RunlengthEncoding:
        type = QString::fromStdString("RLC");
        break;
    default:
        break;
    }
    emit sendInformation(m_information, type);
    emit finished();
}

void CompressorWorker::setRawData(unsigned char *p_rawData){
    m_rawData = p_rawData;
}

void CompressorWorker::setCompData(unsigned char *p_compData){
    m_compData = p_compData;
}

void CompressorWorker::setCodeTable(unsigned char *p_codeTable){
    m_codeTable = p_codeTable;
}

void CompressorWorker::setSizeOfRawData(int p_sizeOfRawData){
    m_sizeOfRawData = p_sizeOfRawData;
}

void CompressorWorker::setSizeOfCompData(int p_sizeOfCompData){
    m_sizeOfCompData = p_sizeOfCompData;
}

void CompressorWorker::setSizeOfCodeTable(int p_sizeOfCodeTable){
    m_sizeOfCodeTable = p_sizeOfCodeTable;
}

void CompressorWorker::setCompressorType(CompressorTypes p_compressorType){
    m_compressorType = p_compressorType;
}

void CompressorWorker::setCurrLang(QString p_currLang){
    m_currLang = p_currLang;
}
