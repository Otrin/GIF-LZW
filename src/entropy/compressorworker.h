#ifndef COMPRESSORWORKER_H
#define COMPRESSORWORKER_H

#include <QObject>

/**
 * @brief Types of compression procedures
 */
enum CompressorTypes{lZW, HUFFMAN, RunlengthEncoding};

/**
 * @brief The CompressorWorker class is needed to calculate and compare the results from the compression procedures
 */
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
    /**
     * @brief this method returns the Compressionrate and the execution time as a QString
     * @param p_information - QString with results of compression (compressionrate + execution time)
     * @param p_type - type of compression procedure
     */
    void sendInformation(QString p_information, QString p_type);
private:
    unsigned char *m_rawData, *m_compData, *m_codeTable;
    int m_sizeOfRawData, m_sizeOfCompData, m_sizeOfCodeTable;
    CompressorTypes m_compressorType;
    QString m_information, m_currLang;
public:
    /**
     * @param p_rawData - data to compress
     */
    void setRawData(unsigned char *p_rawData);
    /**
     * @param p_compData - data to uncompress
     */
    void setCompData(unsigned char *p_sompData);
    void setCodeTable(unsigned char *p_codeTable);
    void setSizeOfRawData(int p_sizeOfRawData);
    void setSizeOfCompData(int p_sizeOfCompData);
    void setSizeOfCodeTable(int p_sizeOfCodeTable);
    /**
     * @brief say which compression procedure you want to use. (default Huffman)
     */
    void setCompressorType(CompressorTypes p_compressorType);
    void setCurrLang(QString p_currLang);
};

#endif // COMPRESSORWORKER_H
