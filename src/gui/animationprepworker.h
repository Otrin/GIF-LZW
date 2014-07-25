#ifndef ANIMATIONPREPWORKER_H
#define ANIMATIONPREPWORKER_H

#include "picture.h"
#include "gif.h"
#include "io.h"
#include <QPixmap>
#include <QPainter>
#include <QObject>

/**
 * @brief This Worker Class is move to a Thread to prepare
 * the necessary QPixmap for animated a GIF
 *
 */
class AnimationPrepWorker : public QObject
{
    Q_OBJECT

private:
    Picture* m_pic;     // Picture to be animated

    /**
     * @brief Creates a Pixmap from a single frame of an animated GIF
     *
     * @param p_frame Frame from animated GIF
     * @return QPixmap Pixmap that was generated from the frame
     */
    QPixmap* generatePixmapFromFrame(Gif *p_gif, int p_frame);
    /**
     * @brief Generates a PixmapArray from gif
     *
     * @param gif GIF that is used to generate the Pixmaps
     * @return QPixmap Pointer to the new Pixmap Array
     */
    QPixmap** generatePixmapArray(Gif *p_gif);

public:
    /**
     * @brief Prepares p_pic to be used for creation of the needed Frames for the Animation
     *
     * @param p_pic Picture to be animated
     */
    AnimationPrepWorker(Picture *p_pic);
    /**
     * @brief Nothing to do here. Deletion is done in MainWindow.
     *
     */
    ~AnimationPrepWorker();

public slots:
    /**
     * @brief Method that is called once this this worker gets the 'moveToThread' treatment
     *
     */
    void process();

signals:
    /**
     * @brief Returns the Pixarray once it is ready
     *
     * @param p_pixArray Array containing the Frames for the Animation
     */
    void pixArrayReady(QPixmap** p_pixArray);
    /**
     * @brief Signals the Thread that work is done
     *
     */
    void finished();
    /**
     * @brief Signals the GUI that an Error occured
     *
     * @param err String containing the Error
     */
    void error(QString err);

};

#endif // ANIMATIONPREPWORKER_H
