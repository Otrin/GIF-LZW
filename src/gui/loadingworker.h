#ifndef LOADINGWORKER_H
#define LOADINGWORKER_H

#include <io.h>
#include <picture.h>
#include <QObject>

class LoadingWorker : public QObject {
    Q_OBJECT

private:
    IO m_ioFile;
    Picture *m_picFromIO;

public:
    LoadingWorker(QString p_filePath);
    ~LoadingWorker();

public slots:
    void process();

signals:
    void finished();
    void picReady(Picture *p_pic);
    void error(QString err);
};

#endif // LOADINGWORKER_H
