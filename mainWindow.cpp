#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "animationThread.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainWindow)
{
    ui->setupUi(this);
    drawPicture();
    drawAnimatedPicture();
}

MainWindow::~MainWindow()
{
    delete[] pixArray;
    delete aT1;
    delete aT2;
    delete aT3;
    delete scene;
    delete scene2;
    delete scene3;
    delete ui;
}

void MainWindow::drawPicture()
{
    QGraphicsScene *scene = new QGraphicsScene(ui->graphicsView_3);
    ui->graphicsView_3->setScene(scene);
    pixArray = new QPixmap[10];
    for(int i = 0; i<10; i++){
        pixArray[i] = generatePixmap(200,200);
    }

    scene->addPixmap(pixArray[0]);
    ui->graphicsView_3->repaint();
}

void MainWindow::drawAnimatedPicture()
{
    scene = new QGraphicsScene(ui->graphicsView_4);
    ui->graphicsView_4->setScene(scene);

    aT1 = new AnimationThread();
    aT1->setFPS(10);
    aT1->setGView(ui->graphicsView_4);
    aT1->setPixArray(pixArray);
    aT1->setScence(scene);
    aT1->start();


    scene2 = new QGraphicsScene(ui->graphicsView_5);
    ui->graphicsView_5->setScene(scene2);

    aT2 = new AnimationThread();
    aT2->setFPS(30);
    aT2->setGView(ui->graphicsView_5);
    aT2->setPixArray(pixArray);
    aT2->setScence(scene2);
    aT2->start();

    scene3 = new QGraphicsScene(ui->graphicsView_6);
    ui->graphicsView_6->setScene(scene3);


    aT3 = new AnimationThread();
    aT3->setFPS(60);
    aT3->setGView(ui->graphicsView_6);
    aT3->setPixArray(pixArray);
    aT3->setScence(scene3);
    aT3->start();
}

QPixmap MainWindow::generatePixmap(/*int *colorTable,*/ int width, int height)
{
    QPixmap pixmap(width,height);
    QPainter p(&pixmap);
    int low = 0;
    int high= 255;
    for (int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++ ){
            p.setPen(QColor((qrand()%high+low),(qrand()%high+low),(qrand()%high+low)));
            p.drawPoint(i, j);
        }

    }
    return pixmap;
}

void MainWindow::repaint()
{
    ui->graphicsView_4->repaint();
}

