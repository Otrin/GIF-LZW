#include "animationThread.h"
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QDebug>

AnimationThread::AnimationThread()
{
    i = 0;
    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(run()));
}

AnimationThread::~AnimationThread()
{
    delete m_timer;
}

void AnimationThread::run(){
    m_graphicsPointer->setPixmap(m_pixArray[i]);
    if(i < m_sizeOfImages){
        i++;
        if(i == m_sizeOfImages)
            i = 0;
    }

    emit repaint(m_gView);
}

void AnimationThread::setGView(QGraphicsView *p_gView){
    m_gView = p_gView;
}

void AnimationThread::setFPS(int p_fps)
{
    m_fps = p_fps;
}

void AnimationThread::setPixArray(QPixmap *p_pixArray)
{
    m_pixArray = p_pixArray;
}

void AnimationThread::resetScence()
{
   // if(m_scene != NULL) delete m_scene;
    m_scene = new QGraphicsScene(m_gView);
    m_gView->setScene(m_scene);
}

void AnimationThread::setSizeOfFrames(int p_sizeOfImages)
{
    m_sizeOfImages = p_sizeOfImages;
}

void AnimationThread::generateGItemPointer()
{
    m_graphicsPointer = m_scene->addPixmap(m_pixArray[0]);
}

void AnimationThread::startAnim()
{
    m_timer->start(100/m_fps);
}

void AnimationThread::stopAnim()
{
    m_timer->stop();
}
