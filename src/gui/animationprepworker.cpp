#include "animationprepworker.h"
#include <QDebug>

AnimationPrepWorker::AnimationPrepWorker(Picture *p_pic)
{
    m_pic = p_pic;
}

AnimationPrepWorker::~AnimationPrepWorker(){

}

void AnimationPrepWorker::process()
{
    Gif *gif = static_cast<Gif*>(m_pic);
    QPixmap **pixArray = generatePixmapArray(gif);
    emit pixArrayReady(pixArray);
    emit finished();
}


QPixmap* AnimationPrepWorker::generatePixmapFromFrame(Gif *p_gif, int p_frame){
    QPixmap *pixmap = new QPixmap(p_gif->getFrame(p_frame)->getWidth(), p_gif->getFrame(p_frame)->getHeight());
    pixmap->fill(Qt::transparent);
    QPainter p(pixmap);
    unsigned char *pixel = p_gif->getFrame(p_frame)->getPixel();
    int counter = 0;
    QColor transpColor;
    if(p_gif->getFrame(p_frame)->getLctFlag() == 1){
        transpColor = QColor(IO::getBit((unsigned int)p_gif->getFrame(p_frame)->getLct()[p_gif->getFrame(p_frame)->getTranspColorIndex()*3],0,8),
                IO::getBit((unsigned int)p_gif->getFrame(p_frame)->getLct()[p_gif->getFrame(p_frame)->getTranspColorIndex()*3+1],0,8),
                IO::getBit((unsigned int)p_gif->getFrame(p_frame)->getLct()[p_gif->getFrame(p_frame)->getTranspColorIndex()*3+2],0,8));
    } else {
        transpColor = QColor(IO::getBit((unsigned int)p_gif->getGCT()[p_gif->getFrame(p_frame)->getTranspColorIndex()*3],0,8),
                IO::getBit((unsigned int)p_gif->getGCT()[p_gif->getFrame(p_frame)->getTranspColorIndex()*3+1],0,8),
                IO::getBit((unsigned int)p_gif->getGCT()[p_gif->getFrame(p_frame)->getTranspColorIndex()*3+2],0,8));
    }
    QColor color;
    if(p_gif->getFrame(p_frame)->getTranspColorFlag() == 1){
        if(p_frame == 0){
            for (int i = 0; i < p_gif->getFrame(p_frame)->getHeight(); i++) {
                for(int j = 0; j < p_gif->getFrame(p_frame)->getWidth(); j++ ){
                    color = QColor(IO::getBit((unsigned int)pixel[counter],0,8),
                            IO::getBit((unsigned int)pixel[counter+1],0,8),
                            IO::getBit((unsigned int)pixel[counter+2],0,8));
                    if(color.red() == transpColor.red()
                            && color.green() == transpColor.green()
                            && color.blue() == transpColor.blue()){
                        color = QColor(255, 255, 255);
                    }
                    p.setPen(color);
                    p.drawPoint(j, i);
                    counter += 3;
                }
            }
        } else {
            for (int i = 0; i < p_gif->getFrame(p_frame)->getHeight(); i++) {
                for(int j = 0; j < p_gif->getFrame(p_frame)->getWidth(); j++ ){
                    color = QColor(IO::getBit((unsigned int)pixel[counter],0,8),
                            IO::getBit((unsigned int)pixel[counter+1],0,8),
                            IO::getBit((unsigned int)pixel[counter+2],0,8));
                    if(color.red() == transpColor.red()
                            && color.green() == transpColor.green()
                            && color.blue() == transpColor.blue()){
                        counter += 3;
                        continue;
                    }
                    p.setPen(color);
                    p.drawPoint(j, i);
                    counter += 3;
                }
            }
        }
    } else {
        for (int i = 0; i < p_gif->getFrame(p_frame)->getHeight(); i++) {
            for(int j = 0; j < p_gif->getFrame(p_frame)->getWidth(); j++ ){
                color = QColor(IO::getBit((unsigned int)pixel[counter],0,8),
                        IO::getBit((unsigned int)pixel[counter+1],0,8),
                        IO::getBit((unsigned int)pixel[counter+2],0,8));
                p.setPen(color);
                p.drawPoint(j, i);
                counter += 3;
            }
        }
    }
    return pixmap;
}

QPixmap** AnimationPrepWorker::generatePixmapArray(Gif *p_gif)
{
    QPixmap **pmArray = (QPixmap**)new QPixmap[p_gif->getSizeOfFrames()];

    for (int i = 0; i < p_gif->getSizeOfFrames(); i++) {
        pmArray[i] = generatePixmapFromFrame(p_gif, i);
    }

    return pmArray;
}
