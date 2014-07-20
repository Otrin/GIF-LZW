#ifndef TABLECONVERSIONWORKER_H
#define TABLECONVERSIONWORKER_H

#include <QObject>
#include "gif.h"
#include "tableconverter.h"

/**
 * @brief This worker converts gifs between local and global color tables.
 *
 */
class TableConversionWorker : public QObject
{
    Q_OBJECT
public:
	enum Mode {Global_to_Local=0, Local_to_Global};

	/**
	 * @brief ConversionStatistics wraps the statistical outcome of the conversion.
	 *
	 *	Values covered are: conversion time, LZW time of original, LZW time of converted,
	 *	filesize of original and filesize of converted.
	 *
	 */
	struct ConversionStatistics{
		double conversionTime = 0;
		double orgLZWTime = 0, newLZWTime = 0;
		int orgSize = 0, newSize = 0;
	};

	TableConversionWorker(Mode p_mode, Gif* p_gif);
    ~TableConversionWorker();

signals:
    void finished();
    void error(QString err);
	void conversionDone(Gif* p_result);
	void statisticsOut(TableConversionWorker::ConversionStatistics* p_statistics);

public slots:
    void process();

private:
	Mode m_mode; // Mode of the conversion.
    Gif* m_gif; // The gif to base the conversion off of

};

#endif // TABLECONVERSIONWORKER_H
