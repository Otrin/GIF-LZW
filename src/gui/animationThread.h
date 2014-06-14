#ifndef ANIMATIONTHREAD_H
#define ANIMATIONTHREAD_H
#include <QThread>
#include <QGraphicsView>
#include <QTimer>
class AnimationThread : public QObject
{
    Q_OBJECT

public:
    AnimationThread();
    ~AnimationThread();
    void setGView(QGraphicsView *gView);
    void setFPS(int fps);
    void setPixArray(QPixmap *pixArray);
    void setScence(QGraphicsScene *scene);
    void generateGItemPointer();
    static int counter ;
    int i;
    void startAnim();

protected slots:
    void run();

signals:
    void repaint();
    void repaint2();
    void repaint3();

private:
    QGraphicsView *gView;
    int fps, id;
    QPixmap *pixArray;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *graphicPointer;
    QTimer *timer;
};
#endif // ANIMATIONTHREAD_H
