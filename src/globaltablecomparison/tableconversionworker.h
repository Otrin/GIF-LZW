#ifndef TABLECONVERSIONWORKER_H
#define TABLECONVERSIONWORKER_H

#include <QObject>
#include "gif.h"

class TableConversionWorker : public QObject
{
    Q_OBJECT
public:
	enum Mode {Global_to_Local =1, Local_to_Global};

	TableConversionWorker(Mode p_mode, Gif* p_gif);
    ~TableConversionWorker();

signals:
    void finished();
    void error(QString err);
    void result(Gif* p_result);

public slots:
    void process();

private:
	Mode m_mode; // Mode of the conversion. 1=global to local, 2=local to global
    Gif* m_gif; // The gif to base the conversion off of

};

#endif // TABLECONVERSIONWORKER_H
