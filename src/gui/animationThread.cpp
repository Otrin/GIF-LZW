#include "animationThread.h"
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QDebug>

AnimationThread::AnimationThread()
{
    m_i = 0;
    m_timer = new QTimer;
    m_timer->setTimerType(Qt::PreciseTimer);
    m_scene = NULL;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(run()));
}

AnimationThread::~AnimationThread()
{
    delete m_timer;
}

void AnimationThread::initPicture(QGraphicsView *p_gView, QPixmap **p_pixArray, int p_sizeOfFrames, int *p_fps)
{
    m_gView = p_gView;
    m_pixArray = p_pixArray;
    m_sizeOfFrames = p_sizeOfFrames;
    m_fps = p_fps;

    resetScence();
    generateGItemPointer();
    resetCounter();
    init = true;
}

void AnimationThread::startAnim()
{
    if(init) m_timer->start(m_fps[0]*10);
}

void AnimationThread::stopAnim()
{
    m_timer->stop();
}

void AnimationThread::run(){
    m_graphicsPointer->setPixmap(*(m_pixArray[m_i]));
    m_timer->setInterval(m_fps[m_i]*10);

    if(m_i < m_sizeOfFrames){
        m_i++;
        if(m_i == m_sizeOfFrames)
            m_i = 0;
    }

    emit repaint(m_gView);
}

void AnimationThread::resetScence()
{
    delete m_scene;
    m_scene = new QGraphicsScene(m_gView);
    m_gView->setScene(m_scene);
}

void AnimationThread::generateGItemPointer()
{
    m_graphicsPointer = m_scene->addPixmap(*(m_pixArray[0]));
}

void AnimationThread::resetCounter()
{
    m_i = 0;
}
