#ifndef COMPRESSORWORKER_H
#define COMPRESSORWORKER_H

#include <QObject>

enum CompressorTypes{lZW, HUFFMAN, RunlengthEncoding};

class CompressorWorker: public QObject{
private:
    Q_OBJECT
public:
    CompressorWorker();
    ~CompressorWorker();
public slots:
    void process();
signals:
    void finished();
    void error(QString err);
    void sendInformation(QString p_information, QString p_type);
private:
    unsigned char *m_rawData, *m_compData, *m_codeTable;
    int m_sizeOfRawData, m_sizeOfCompData, m_sizeOfCodeTable;
    CompressorTypes m_compressorType;
    QString m_information;
public:
    void setRawData(unsigned char *p_rawData);
    void setCompData(unsigned char *p_sompData);
    void setCodeTable(unsigned char *p_codeTable);
    void setSizeOfRawData(int p_sizeOfRawData);
    void setSizeOfCompData(int p_sizeOfCompData);
    void setSizeOfCodeTable(int p_sizeOfCodeTable);
    void setCompressorType(CompressorTypes p_compressorType);
};

#endif // COMPRESSORWORKER_H
