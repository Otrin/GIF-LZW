#include "animationThread.h"
#include <QDebug>
#include <QGraphicsPixmapItem>

int AnimationThread::counter = 0;

AnimationThread::AnimationThread()
{
// no new in here -> destroys QTs Thread structure
    id = counter;
    counter++;
}

AnimationThread::~AnimationThread()
{

}


void AnimationThread::process()
{
    QGraphicsPixmapItem *graphicPointer = scene->addPixmap(pixArray[0]);
    while(1){
        for (int i = 0; i < 10; i++) {
            graphicPointer->setPixmap(pixArray[i]);
            if(id == 0)
                emit repaint();
            else
                if(id == 1)
                    emit repaint2();
                else
                    emit repaint3();
            //QThread::msleep(1000/fps);
        }
    }
    emit finished();
}


void AnimationThread::setGView(QGraphicsView *gView){
    this->gView = gView;
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
