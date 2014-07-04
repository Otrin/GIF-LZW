#ifndef ANIMATIONPREPWORKER_H
#define ANIMATIONPREPWORKER_H

#include "picture.h"
#include "gif.h"
#include "io.h"
#include <QPixmap>
#include <QPainter>
#include <QObject>

class AnimationPrepWorker : public QObject
{
    Q_OBJECT

private:
    QPixmap* generatePixmapFromFrame(Frame *p_frame);
    QPixmap** generatePixmapArray(Gif *gif);
    Picture* m_pic;

public:
    AnimationPrepWorker(Picture *p_pic);
    ~AnimationPrepWorker();

public slots:
    void process();

signals:
    void pixArrayReady(QPixmap** p_pixArray);
    void finished();
    void error(QString err);

};

#endif // ANIMATIONPREPWORKER_H
