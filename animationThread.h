#ifndef ANIMATIONTHREAD_H
#define ANIMATIONTHREAD_H

#include <QThread>
#include <QGraphicsView>

class AnimationThread : public QThread
{
    Q_OBJECT
public:
    explicit AnimationThread(QObject *parent = 0);
    void setGView(QGraphicsView *gView);
    void setFPS(int fps);
    void setPixArray(QPixmap *pixArray);
    void setScence(QGraphicsScene *scene);
signals:

public slots:

private:
    void run();
    QGraphicsView *gView;
    int fps;
    QPixmap *pixArray;
    QGraphicsScene *scene;

signals:
    void repaint();
};

#endif // ANIMATIONTHREAD_H
