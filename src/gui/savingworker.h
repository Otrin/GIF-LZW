#ifndef SAVINGWORKER_H
#define SAVINGWORKER_H


#include <io.h>
#include <picture.h>
#include <QObject>
#include <QImage>
#include <QColor>

/**
 * @brief This Worker Class is moved to a Thread to save a Picture
 * without freezing the whole GUI
 *
 */
class SavingWorker : public QObject {
    Q_OBJECT

private:
    QString m_filePath; /**< Path where the GIF is saved */
    int m_mode; /**< What kind of Data is saved. 0 = GIF, 1 = Picture, 2 = Raw */
    Picture *m_picFromIO; /**< GIF to save as a GIF */
    QImage m_qImgFromIO; /**< Picture to save as a GIF */
    unsigned char *m_rawData; /**< Raw Data to save as a GIF */
    int m_rawDataSize; /**< Size of m_rawData */

public:
    /**
     * @brief Saves a GIF at p_filePath specified by p_mode;
     * 0 = GIF
     * 1 = Picture
     * 2 = Rawdata
     * Use the needed setters at your own responsibility, before starting the Thread
     *
     * @param p_filePath
     * @param p_mode
     */
    SavingWorker(QString p_filePath, int p_mode);
    /**
     * @brief Nothing to do here. Deletion is done in MainWindow.
     *
     */
    ~SavingWorker();
    /**
     * @brief Setter Method
     *
     * @param picFromIO Picture to set
     */
    void setPicFromIO(Picture *picFromIO);
    /**
     * @brief Setter Method
     *
     * @param qImgFromIO QImage to set
     */
    void setQImgFromIO(const QImage &qImgFromIO);
    /**
     * @brief Setter Method
     *
     * @param rawData RawData to set
     */
    void setRawData(unsigned char *rawData);
    /**
     * @brief Setter Method
     *
     * @param rawDataSize Size of raw Data to set
     */
    void setRawDataSize(int rawDataSize);

public slots:
    /**
     * @brief Method that is called once this this worker gets the 'moveToThread' treatment
     *
     */
    void process();

signals:
    /**
     * @brief Signals the Thread that work is done
     *
     */
    void finished();
    /**
     * @brief Signals that saving ist done
     *
     */
    void savingDone();
    /**
     * @brief Signals the GUI that an Error occured
     *
     * @param err String containing the Error
     */
    void error(QString err);
};

#endif // SAVINGWORKER_H
