#include "animationThread.h"
#include <QDebug>
#include <QGraphicsPixmapItem>

AnimationThread::AnimationThread(QObject *parent) :
    QThread(parent)
{
}

void AnimationThread::setFPS(int fps)
{
    this->fps = fps;
}

void AnimationThread::setPixArray(QPixmap *pixArray)
{
    this->pixArray = pixArray;
}

void AnimationThread::setScence(QGraphicsScene *scene)
{
    this->scene = scene;
}

void AnimationThread::setGView(QGraphicsView *gView){
    this->gView = gView;
}

void AnimationThread::run()
{
    QGraphicsPixmapItem *graphicPointer = scene->addPixmap(pixArray[0]);
    while(1){
        for (int i = 0; i < 10; i++) {
            graphicPointer->setPixmap(pixArray[i]);
            emit repaint();
            msleep(1000/fps);
        }
    }
}
