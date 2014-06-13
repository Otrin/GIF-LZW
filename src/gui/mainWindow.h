#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "animationThread.h"
#include <QMainWindow>
#include <QPixmap>

namespace Ui {
class mainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::mainWindow *ui;
    QPixmap *pixArray;
    QPixmap generatePixmap(/*int *colorTable,*/ int width, int height);
    AnimationThread *aT1, *aT2, *aT3;
    QGraphicsScene *scene, *scene2, *scene3;
    void drawPicture();
    void drawAnimatedPicture();
    void guiSetup();

private slots:
    void repaint(); //Kein repaint aus Thread -> hier slot mit repaint funktion in Thread signal das per emit ausgelöst wird
};

#endif // MAINWINDOW_H
