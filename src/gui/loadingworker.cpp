#include "loadingworker.h"
#include <QDebug>

LoadingWorker::LoadingWorker(QString p_filePath)
{

    const QByteArray asc = p_filePath.toLocal8Bit();
    m_ioFile = new IO(std::string(asc.constData(), asc.length()));
}

LoadingWorker::~LoadingWorker()
{

}

void LoadingWorker::process()
{
    m_ioFile->loadFile();
    m_picFromIO = m_ioFile->getGif();

    emit picReady(m_picFromIO);
    emit ioReady(m_ioFile);
    emit finished();
}
