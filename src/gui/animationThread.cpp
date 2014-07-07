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

void AnimationThread::initPicture(Gif *p_gif, QGraphicsView *p_gView, QPixmap **p_pixArray, int p_sizeOfFrames, int *p_fps)
{
    m_gView = p_gView;
    m_pixArray = p_pixArray;
    m_sizeOfFrames = p_sizeOfFrames;
    m_fps = p_fps;
    m_gif = p_gif;

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
    QPainter painter(&m_pixmap);
    painter.drawPixmap(m_gif->getFrame(m_i)->getLeft(), m_gif->getFrame(m_i)->getTop(),
                       m_gif->getFrame(m_i)->getWidth(), m_gif->getFrame(m_i)->getHeight(),
                       *(m_pixArray[m_i]));
    m_graphicsPointer->setPixmap(m_pixmap);
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
    m_pixmap = QPixmap(m_gif->getFrame(0)->getWidth(), m_gif->getFrame(0)->getHeight());
    QPainter painter(&m_pixmap);
    QColor color(255,255,255);
    for (int i = 0; i < m_gif->getFrame(0)->getHeight(); ++i) {
        for (int j = 0; j < m_gif->getFrame(0)->getWidth(); ++j) {
            painter.setPen(color);
            painter.drawPoint(j, i);
        }
    }
    m_scene = new QGraphicsScene(m_gView);
    m_scene->addPixmap(m_pixmap);
    m_gView->setScene(m_scene);
}

void AnimationThread::generateGItemPointer()
{
    m_graphicsPointer = m_scene->addPixmap(m_pixmap);
}

void AnimationThread::resetCounter()
{
    m_i = 0;
}
