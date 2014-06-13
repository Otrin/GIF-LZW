#ifndef ANIMATIONTHREAD_H
#define ANIMATIONTHREAD_H

#include <QThread>
#include <QGraphicsView>

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
    static int counter;

public slots:
    void process();

signals:
    void finished();
    void error(QString err);
    void repaint();
    void repaint2();
    void repaint3();

private:
    QGraphicsView *gView;
    int fps, id;
    QPixmap *pixArray;
    QGraphicsScene *scene;
};

#endif // ANIMATIONTHREAD_H
