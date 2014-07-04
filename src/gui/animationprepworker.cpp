#include "animationprepworker.h"

AnimationPrepWorker::AnimationPrepWorker(Picture *p_pic)
{
    m_pic = p_pic;
}

AnimationPrepWorker::~AnimationPrepWorker(){

}

void AnimationPrepWorker::process()
{
    Gif* gif = static_cast<Gif*>(m_pic);
    QPixmap ** pixArray = generatePixmapArray(gif);
    emit pixArrayReady(pixArray);
}


QPixmap* AnimationPrepWorker::generatePixmapFromFrame(Frame *p_frame){
    QPixmap *pixmap = new QPixmap(p_frame->getWidth(), p_frame->getHeight());
    QPainter p(pixmap);
    char *pixel = p_frame->getPixel();
    int counter = 0;

    QColor color;
    for (int i = 0; i < p_frame->getHeight(); i++) {
        for(int j = 0; j < p_frame->getWidth(); j++ ){
            color = QColor(IO::getBit((unsigned int)pixel[counter],0,8),
                    IO::getBit((unsigned int)pixel[counter+1],0,8),
                    IO::getBit((unsigned int)pixel[counter+2],0,8));
            p.setPen(color);
            p.drawPoint(j, i);

            counter += 3;
        }

    }
    return pixmap;
}

QPixmap** AnimationPrepWorker::generatePixmapArray(Gif *gif)
{
    QPixmap **pmArray = (QPixmap**)new QPixmap[gif->getSizeOfFrames()];

    for (int i = 0; i < gif->getSizeOfFrames(); i++) {
        pmArray[i] = generatePixmapFromFrame(gif->getFrame(i));
    }

    return pmArray;
}
