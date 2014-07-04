#ifndef ANIMATIONTHREAD_H
#define ANIMATIONTHREAD_H
#include <QThread>
#include <QGraphicsView>
#include <QTimer>
#include <QGraphicsPixmapItem>

/**
 * @brief Used to display an animated GIF onto a QGraphicsView
 * This class is not really a Thread, instead it derives from QObjet and
 * uses a QTimer to fullfill the functionality of Thread.sleep.
 * This Usage was adviced in the QT Documentation
 *
 * After construction make sure to call initPicture(...) before startAnim()
 *
 */
class AnimationThread : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Initializes a new QTimer. Make sure to call initPicture before startAnim();
     *
     */
    AnimationThread();
    /**
     * @brief Cleans up the resources occupied by m_timer
     *
     */
    ~AnimationThread();
    /**
     * @brief Initializes all the required parameters to display the Frames from p_pixArray with speed of p_fps onto p_gView
     *
     * @param p_gView
     * @param p_pixArray
     * @param p_sizeOfFrames
     * @param p_fps
     */
    void initPicture(QGraphicsView *p_gView, QPixmap **p_pixArray, int p_sizeOfFrames, int *p_fps);
    /**
     * @brief Starts animating with the parameters set by calling initPicture()
     *
     */
    void startAnim();
    /**
     * @brief Stops animating. Calling startAnim() again will begin from the first frame
     *
     */
    void stopAnim();

protected slots:
    /**
     * @brief Worker method to display the frames with delay to the QGraphicsview
     *
     */
    void run();

signals:
    /**
     * @brief Signals p_gView to call its repaint Method
     *
     * @param p_gView QGraphicsview that gets repainted
     */
    void repaint(QGraphicsView *p_gView);

private:
    int m_i; // This magical integer has the glorious destiny of being incremented till it reaches m_sizeOfFrames
    int *m_fps; // Array that contains the delayTimes for animation
    int m_sizeOfFrames; // Number of images in m_pixArray
    bool init; // True if initPicture was called
    QGraphicsView *m_gView; // View that displays the frames from m_pixArray
    QGraphicsScene *m_scene; // Scene that is in m_gView
    QGraphicsPixmapItem *m_graphicsPointer; // Pointer to the pixmap in m_scene in m_gView that gets the next Picture to display.
    QPixmap **m_pixArray; // Array of Pixmap to animate
    QTimer *m_timer; // Calls the run() Method to animate the picture

    /**
     * @brief Sets a new QGraphicsscene onto m_gView for a fresh canvas to paint on
     *
     */
    void resetScence();
    /**
     * @brief Pointer to the pixmap in m_scene in m_gView that gets the next Picture to display.
     *
     */
    void generateGItemPointer();
    /**
     * @brief This beautiful method has the noble Task of resetting m_i to 0. It is really compact, therefore runs really well -> many science
     *
     */
    void resetCounter();
};
#endif // ANIMATIONTHREAD_H
