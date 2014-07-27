#include "animationThread.h"
#include "io.h"
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
    m_init = true;
}

void AnimationThread::startAnim()
{
	if(m_init) m_timer->start(0);
}

void AnimationThread::stopAnim()
{
    m_timer->stop();
}

void AnimationThread::run(){
	if(m_pixArray[m_i] == NULL){
		m_i++;
		if(m_i == m_sizeOfFrames)
			m_i = 0;
	}

    QPainter painter(&m_pixmap);
    if(m_i > 0 && m_gif->getFrame(m_i-1)->getDisposualMethod() == 2){
        QColor bgColor(255, 255, 255);
        painter.fillRect(0,0,m_gif->getWidth(), m_gif->getHeight(), bgColor);
    }
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

void AnimationThread::setGView(QGraphicsView *p_gView)
{
    m_gView = p_gView;
}

void AnimationThread::resetCounter()
{
    m_i = 0;
}
