#include "loadingworker.h"
#include <QDebug>

LoadingWorker::LoadingWorker(QString p_filePath)
{
    m_ioFile = new IO(p_filePath.toStdString());
}

LoadingWorker::~LoadingWorker()
{
    delete m_ioFile;
}

void LoadingWorker::process()
{
    m_ioFile->loadFile();
    m_picFromIO = m_ioFile->getGif();

    emit picReady(m_picFromIO);
    emit finished();
}
