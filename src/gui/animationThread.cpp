#include "animationThread.h"
#include <QGraphicsPixmapItem>
#include <QTimer>
int AnimationThread::counter = 0;

AnimationThread::AnimationThread()
{
    id = counter;
    counter++;
    i = 0;
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(run()));
}

AnimationThread::~AnimationThread()
{
    delete timer;
}

void AnimationThread::run(){
    graphicPointer->setPixmap(pixArray[i]);
    if(i < 9){
        i++;
    }
    else{
         i = 0;
    }
    if(id == 0)
        emit repaint();
    else
        if(id == 1)
            emit repaint2();
        else
            emit repaint3();
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

void AnimationThread::generateGItemPointer()
{
    graphicPointer = scene->addPixmap(pixArray[0]);
}

void AnimationThread::startAnim()
{
    timer->start(1000/fps);
}

void AnimationThread::stopAnim()
{
    timer->stop();
}
