#include "savingworker.h"
#include <QDebug>


SavingWorker::SavingWorker(QString p_filePath, int p_mode)
{
    m_filePath = p_filePath;
    m_mode = p_mode;
    m_picFromIO = NULL;
    m_rawData = NULL;
    m_rawDataSize = 0;
}

SavingWorker::~SavingWorker()
{

}

void SavingWorker::process()
{
    switch (m_mode) {
    case 0: //GIF
    {
        IO gifIOFile(m_filePath.toStdString());
        gifIOFile.saveGif(*(static_cast<Gif*>(m_picFromIO)));
        break;
    }
    case 1:  //PICTURE
    {
        Gif gif;
        gif.setHeight(m_qImgFromIO.height());
        gif.setWidth(m_qImgFromIO.width());
        gif.extendFrames();
        gif.getFrame(0)->setHeight(m_qImgFromIO.height());
        gif.getFrame(0)->setWidth(m_qImgFromIO.width());

        unsigned char* pix = new unsigned char[m_qImgFromIO.height()*m_qImgFromIO.width()*3];

        int k = 0;
        for (int i = 0; i < m_qImgFromIO.height(); ++i) {
            for (int j = 0; j < m_qImgFromIO.width(); ++j) {
                QColor rgb = QColor(m_qImgFromIO.pixel(j,i));
                pix[k] = (unsigned char)rgb.red();
                pix[k+1] = (unsigned char)rgb.green();
                pix[k+2] = (unsigned char)rgb.blue();
                k+=3;
            }
        }

        gif.getFrame(0)->setPixel(pix, m_qImgFromIO.height()*m_qImgFromIO.width()*3);
        gif.getFrame(0)->setDataFlag(0);
        gif.getFrame(0)->setLctFlag(1);
        IO::generateRawData(gif, 0, 1);

        IO gifIOFile(m_filePath.toStdString());
        gifIOFile.saveGif(gif);
        break;
    }
    case 2: //RAW
    {
        Gif gif;
        gif.setHeight(m_rawDataSize/3);
        gif.setWidth(1);
        gif.extendFrames();
        gif.getFrame(0)->setHeight(m_rawDataSize/3);
        gif.getFrame(0)->setWidth(1);
        gif.getFrame(0)->setPixel(m_rawData, m_rawDataSize);
        gif.getFrame(0)->setDataFlag(0);
        gif.getFrame(0)->setLctFlag(1);
        IO::generateRawData(gif, 0, 1);

        IO gifIOFile(m_filePath.toStdString());
        gifIOFile.saveGif(gif);
        break;
    }
    default:
        break;
    }

    emit savingDone();
    emit finished();
}




void SavingWorker::setPicFromIO(Picture *picFromIO)
{
    m_picFromIO = picFromIO;
}


void SavingWorker::setQImgFromIO(const QImage &qImgFromIO)
{
    m_qImgFromIO = qImgFromIO;
}



void SavingWorker::setRawData(unsigned char *rawData)
{
    m_rawData = rawData;
}


void SavingWorker::setRawDataSize(int rawDataSize)
{
    m_rawDataSize = rawDataSize;
}
