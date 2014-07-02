#ifndef ANIMATIONTHREAD_H
#define ANIMATIONTHREAD_H
#include <QThread>
#include <QGraphicsView>
#include <QTimer>
#include <QGraphicsPixmapItem>

class AnimationThread : public QObject
{
    Q_OBJECT

public:
    AnimationThread();
    ~AnimationThread();
    void setGView(QGraphicsView *p_gView);
    void setFPS(int p_fps);
    void setPixArray(QPixmap *p_pixArray);
    void setScence(QGraphicsScene *p_scene);
    void setSizeOfImages(int p_sizeOfImages);
    void generateGItemPointer();
    void startAnim();
    void stopAnim();

protected slots:
    void run();

signals:
    void repaint(QGraphicsView *p_gView);

private:
    QGraphicsView *m_gView;
    int i, m_fps, m_sizeOfImages;
    QPixmap *m_pixArray;
    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_graphicsPointer;
    QTimer *m_timer;
};
#endif // ANIMATIONTHREAD_H
