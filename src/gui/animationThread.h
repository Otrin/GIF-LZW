#ifndef ANIMATIONTHREAD_H
#define ANIMATIONTHREAD_H

#include <QThread>
#include <QGraphicsView>

class AnimationThread : public QThread
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


protected slots:
    void run();

signals:
    void error(QString err);
    void repaint();
    void repaint2();
    void repaint3();

private:
    QGraphicsView *gView;
    int fps, id;
    QPixmap *pixArray;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *graphicPointer;
};

#endif // ANIMATIONTHREAD_H
