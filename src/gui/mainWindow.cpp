#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "animationThread.h"
#include <iostream>
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QUrl>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainWindow)
{
    ui->setupUi(this);
    dropSetup();
    setAcceptDrops(true);
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    createLanguageMenu();
    loadLanguage("de");
    drawPicture();
    drawAnimatedPicture();
    guiSetup();
    ui->tab1_graphicsView_1->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->tabWidget->setCurrentIndex(0);
    m_tabPosition = 0;
}

MainWindow::~MainWindow()
{
    delete[] pixArray;
    delete p_scene;
    delete scene2;
    delete scene3;
    delete ui;
    if(m_picFromIO != NULL) delete m_picFromIO;
    if(m_aboutDialog != NULL) delete m_aboutDialog;
    if(m_instructionDialog != NULL) delete m_instructionDialog;
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
    p_scene = new QGraphicsScene(ui->graphicsView_4);
    ui->graphicsView_4->setScene(p_scene);

    aT1.setFPS(10);
    aT1.setGView(ui->graphicsView_4);
    aT1.setPixArray(pixArray);
    aT1.setScence(p_scene);
    aT1.generateGItemPointer();
    aT1.startAnim();


    scene2 = new QGraphicsScene(ui->graphicsView_5);
    ui->graphicsView_5->setScene(scene2);

    aT2.setFPS(30);
    aT2.setGView(ui->graphicsView_5);
    aT2.setPixArray(pixArray);
    aT2.setScence(scene2);
    aT2.generateGItemPointer();
    aT2.startAnim();


    scene3 = new QGraphicsScene(ui->graphicsView_6);
    ui->graphicsView_6->setScene(scene3);

    aT3.setFPS(60);
    aT3.setGView(ui->graphicsView_6);
    aT3.setPixArray(pixArray);
    aT3.setScence(scene3);
    aT3.generateGItemPointer();
    aT3.startAnim();
}

void MainWindow::guiSetup()
{
    if(loadPicture(QString("rsc/startup.gif"))){
        displayPicture(ui->tab1_graphicsView_1, m_drawPicture);
        displayHeaderInfo(ui->tab1_textEdit_1, m_picFromIO);
    }
}

void MainWindow::dropSetup(){
    ui->tab1_graphicsView_1->setAcceptDrops(false);
    ui->tab1_textEdit_1->setAcceptDrops(false);
    ui->tab2_graphicsView_1->setAcceptDrops(false);
    ui->tab2_graphicsView_2->setAcceptDrops(false);
    ui->tab2_horizontalSlider_1->setAcceptDrops(false);
    ui->tab2_pushButton_1->setAcceptDrops(false);
    ui->tab2_pushButton_2->setAcceptDrops(false);
    ui->tab3_graphicsView_1->setAcceptDrops(false);
    ui->tab3_graphicsView_2->setAcceptDrops(false);
    ui->tab3_label_1->setAcceptDrops(false);
    ui->tab3_label_2->setAcceptDrops(false);
    ui->tab3_textEdit_1->setAcceptDrops(false);
    ui->tab3_textEdit_2->setAcceptDrops(false);
    ui->tab4_label_1->setAcceptDrops(false);
    ui->tab4_label_2->setAcceptDrops(false);
    ui->tab4_label_3->setAcceptDrops(false);
    ui->tab4_textEdit_1->setAcceptDrops(false);
    ui->tab4_textEdit_2->setAcceptDrops(false);
    ui->tab4_textEdit_3->setAcceptDrops(false);
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

// we create the menu entries dynamically, dependant on the existing translations.
void MainWindow::createLanguageMenu(void)
{
    QActionGroup* langGroup = new QActionGroup(ui->menuLanguage);
    langGroup->setExclusive(true);

    connect(langGroup, SIGNAL(triggered(QAction *)), this, SLOT(slotLanguageChanged(QAction *)));

    // format systems language
    QString defaultLocale = QLocale::system().name();       // e.g. "de_DE"
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // e.g. "de"

    m_langPath = QApplication::applicationDirPath();
    m_langPath.append("/lang");
    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList("Translation_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i)
    {

        // get locale extracted by filename
        QString locale;
        locale = fileNames[i];                  // "TranslationExample_de.qm"
        locale.truncate(locale.lastIndexOf('.'));   // "TranslationExample_de"
        locale.remove(0, locale.indexOf('_') + 1);   // "de"

        QString lang = QLocale::languageToString(QLocale(locale).language());
        QIcon ico(QString("%1/%2.png").arg(m_langPath).arg(locale));

        QAction *action = new QAction(ico, lang, this);
        action->setCheckable(true);
        action->setData(locale);

        ui->menuLanguage->addAction(action);
        langGroup->addAction(action);

        // set default translators and language checked
        if (defaultLocale == locale)
        {
            action->setChecked(true);
        }
    }
}

QPixmap MainWindow::generatePixmapFromPicture(Picture *p_pic)
{
    QPixmap backUp(p_pic->getWidth(), p_pic->getHeight());
    Gif* gif = static_cast<Gif*>(p_pic);
    if(gif != 0) {
        QPixmap pixmap(gif->geFrame(0)->getWidth(), gif->geFrame(0)->getHeight());
        QPainter p(&pixmap);
        char *pixel = gif->geFrame(0)->getPixel();
        int counter = 0;

        QColor color;
        for (int i = 0; i < gif->geFrame(0)->getHeight(); i++) {
            for(int j = 0; j < gif->geFrame(0)->getWidth(); j++ ){
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
    return backUp;

}

bool MainWindow::loadPicture(QString p_filePath){
    if(p_filePath.endsWith(".gif")){  //Picture is a GIF
        m_ioFile = IO(p_filePath.toStdString());
        m_ioFile.loadFile();
        m_picFromIO = m_ioFile.getGif();
        m_drawPicture = generatePixmapFromPicture(m_picFromIO);
        displayPicture(ui->tab1_graphicsView_1, m_drawPicture);
        displayHeaderInfo(ui->tab1_textEdit_1, m_picFromIO);
        m_picIsGIF = true;
        return true;
    }
    else{           //Picture is NOT a GIF
        // if(m_picFromIO != NULL) delete m_picFromIO;      //wanted to delete previously loaded GIF -> free error: invalid Pointer
        m_qImgFromIO = QImage(p_filePath);
        m_drawPicture = QPixmap::fromImage(m_qImgFromIO);
        m_picIsGIF = false;
        return true;
    }
    return false;
}


void MainWindow::displayPicture(QGraphicsView *view, QPixmap &pic)
{
    QGraphicsScene *scene = new QGraphicsScene(view);
    view->setScene(scene);

    scene->addPixmap(pic);
    view->repaint();

    scalePicture(view, scene, pic.width());
}

void MainWindow::displayHeaderInfo(QTextEdit *p_textEdit, Picture *p_picFromIO)
{
    Gif* headerInfo = static_cast<Gif*>(p_picFromIO);
    if(headerInfo != 0) {
        m_headerInfo = "";
        m_headerInfo.append(QString("Width: %1 px\n").arg(headerInfo->getWidth()));
        m_headerInfo.append(QString("Height: %1 px\n\n").arg(headerInfo->getHeight()));
        m_headerInfo.append(QString("GCT Flag: %1\n").arg(headerInfo->getGctFlag()));
        m_headerInfo.append(QString("GCT Size: %1\n").arg(headerInfo->getSizeOfGCT()));
        m_headerInfo.append(QString("BG Color: %1\n").arg(headerInfo->getBgColor()));
        m_headerInfo.append(QString("Frames: %1\n").arg(headerInfo->getSizeOfImages()));
        m_headerInfo.append("\n");
        m_headerInfo.append("Interlace-Flag: ");
        m_headerInfo.append(QString("%1").arg(headerInfo->geFrame(0)->getInterlaceFlag()));

        p_textEdit->setText(m_headerInfo);
    }
}

void MainWindow::displayHeaderInfo(QTextEdit *p_textEdit, QImage &p_qImgFromIO)
{
    m_headerInfo = "";
    m_headerInfo.append(QString("Width: %1 px\n").arg(p_qImgFromIO.width()));
    m_headerInfo.append(QString("Height: %1 px\n\n").arg(p_qImgFromIO.height()));

    m_headerInfo.append(QString("GCT Size: %1\n").arg(p_qImgFromIO.colorCount()));


    p_textEdit->setText(m_headerInfo);
}

// Called every time, when a menu entry of the language menu is called
void MainWindow::slotLanguageChanged(QAction* action)
{
    if(0 != action)
    {
        // load the language dependant on the action content
        loadLanguage(action->data().toString());
        setWindowIcon(action->icon());
    }
}

void MainWindow::errorString(QString error)
{
    qDebug() << error;
}

void switchTranslator(QTranslator& translator, const QString& filename)
{
    // remove the old translator
    qApp->removeTranslator(&translator);

    // load the new translator
    if(translator.load(filename))
        qApp->installTranslator(&translator);
}

void MainWindow::loadLanguage(const QString& rLanguage)
{
    if(m_currLang != rLanguage)
    {
        m_currLang = rLanguage;
        QLocale locale = QLocale(m_currLang);
        QLocale::setDefault(locale);
        switchTranslator(m_translator, QString("lang/Translation_%1.qm").arg(rLanguage)); //load new Translator from Folder "lang"
    }
}

void MainWindow::changeEvent(QEvent* event)
{
    if(0 != event)
    {
        switch(event->type())
        {
            // this event is send if a translator is loaded
            case QEvent::LanguageChange:
              {
                ui->retranslateUi(this);
                break;
              }
            // this event is send, if the system, language changes
            case QEvent::LocaleChange:
              {
                QString locale = QLocale::system().name();
                locale.truncate(locale.lastIndexOf('_'));
                loadLanguage(locale);
                break;
               }
            default:
                break;
      }
    }

    QMainWindow::changeEvent(event);
}


void MainWindow::repaint()
{
    ui->graphicsView_4->repaint();
}

void MainWindow::repaint2()
{
    ui->graphicsView_5->repaint();
}

void MainWindow::repaint3()
{
    ui->graphicsView_6->repaint();
}

void MainWindow::keyPressEvent(QKeyEvent *event){
   switch(event->key()){
   case Qt::Key_Space:
       if(event->modifiers() & Qt::ControlModifier){
           if(m_tabPosition+1 < 4)
               m_tabPosition++;
           else
               m_tabPosition = 0;

           ui->tabWidget->setCurrentIndex(m_tabPosition);
       }
       break;
   case Qt::Key_Plus:
        if(event->modifiers() & Qt::ControlModifier){
            double scaleFactor = 1.30;
            ui->tab1_graphicsView_1->scale(scaleFactor, scaleFactor);
        }
        break;
   case Qt::Key_Minus:
        if(event->modifiers() & Qt::ControlModifier){
           double scaleFactor = 1.30;
           ui->tab1_graphicsView_1->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
        break;
   case Qt::Key_0:
       if(event->modifiers() & Qt::ControlModifier){
           ui->tab1_graphicsView_1->setTransform(QTransform::fromScale(1.0, 1.0));
       }
       break;
   case Qt::Key_Period:
       if(event->modifiers() & Qt::ControlModifier){
           ui->tab1_graphicsView_1->fitInView(ui->tab1_graphicsView_1->scene()->sceneRect(), Qt::KeepAspectRatio);
       }
       break;
   default:
       break;
   }
}

void MainWindow::wheelEvent(QWheelEvent* event) {
    double scaleFactor = 1.20;
    if(event->angleDelta().y() > 0) {
        // Zoom in
        ui->tab1_graphicsView_1->scale(scaleFactor, scaleFactor);
    } else {
        // Zoom out
        ui->tab1_graphicsView_1->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void MainWindow::closeEvent(QCloseEvent *event){
    aT1.stopAnim();
    aT2.stopAnim();
    aT3.stopAnim();
    event->accept();
    exit(0);
}

void MainWindow::scalePicture(QGraphicsView *p_view, QGraphicsScene *p_scene, int p_pictureWidth)
{
    p_view->setTransform(QTransform::fromScale(1.0, 1.0)); //reset scale

    if(p_pictureWidth < 50)
       p_view->setTransform(QTransform::fromScale(2.0, 2.0));
    else
       if(p_pictureWidth < 100)
            p_view->setTransform(QTransform::fromScale(1.5, 1.5));
       else
           if(p_pictureWidth < 200)
               p_view->setTransform(QTransform::fromScale(1.3, 1.3));
           else
               if(p_pictureWidth < 300)
                    p_view->setTransform(QTransform::fromScale(1.2, 1.2));
               else
                   if(p_pictureWidth > p_view->rect().width()){
                       p_view->fitInView(p_scene->sceneRect(), Qt::KeepAspectRatio);    //Zooms Picture to fit the View
                   }
}

void MainWindow::on_actionBeenden_triggered()
{
    aT1.stopAnim();
    aT2.stopAnim();
    aT3.stopAnim();
    exit(0);
}

void MainWindow::on_actionDatei_ffnen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                                     tr("Image Files (*.gif *.png *.jpg *.jpeg *.bmp *.tiff)"));
    if(fileName != NULL){
        if(loadPicture(fileName)){
            displayPicture(ui->tab1_graphicsView_1, m_drawPicture);

            if(m_picIsGIF)
                displayHeaderInfo(ui->tab1_textEdit_1, m_picFromIO);
            else
                displayHeaderInfo(ui->tab1_textEdit_1, m_qImgFromIO);

            ui->tabWidget->setCurrentIndex(0);  //Displays first Tab
        }
    }
}

void MainWindow::on_actionGIF_Bild_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                     tr("GIF (*.gif*)"));

    if(!fileName.contains(".gif")) fileName.append(".gif");
    qDebug() << fileName;

    //Here needs to be IO Code to save the File
}

void MainWindow::on_actionLokale_Globale_Tabellen_Vergleichsbild_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                     tr("GIF (*.gif*)"));

    if(!fileName.contains(".gif")) fileName.append(".gif");
    qDebug() << fileName;

    //Here needs to be IO Code to save the File
}

void MainWindow::on_action_ber_triggered()
{
    if(m_aboutDialog != NULL){
        m_aboutDialog = new AboutDialog;
    }

    m_aboutDialog->show();
}

void MainWindow::on_actionAnleitung_triggered()
{
    if(m_instructionDialog != NULL){
        m_instructionDialog = new InstructionDialog;
    }

    m_instructionDialog->show();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    foreach(QUrl url, event->mimeData()->urls())
        if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="GIF"){
            event->acceptProposedAction();
            return;
        } else
            if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="JPG"){
                event->acceptProposedAction();
                return;
            } else
                if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="JPEG"){
                    event->acceptProposedAction();
                    return;
                } else
                    if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="PNG"){
                        event->acceptProposedAction();
                        return;
                    } else
                        if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="BMP"){
                            event->acceptProposedAction();
                            return;
                        } else
                            if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="TIFF"){
                                event->acceptProposedAction();
                                return;
                            }
}

void MainWindow::dropEvent(QDropEvent *event)
 {
     foreach(QUrl url, event->mimeData()->urls()){
         if(loadPicture(url.toLocalFile())){
             displayPicture(ui->tab1_graphicsView_1, m_drawPicture);

             if(m_picIsGIF)
                 displayHeaderInfo(ui->tab1_textEdit_1, m_picFromIO);
             else
                 displayHeaderInfo(ui->tab1_textEdit_1, m_qImgFromIO);

             ui->tabWidget->setCurrentIndex(0);  //Displays first Tab
         }
         return;  //only one file accepted
     }

     event->acceptProposedAction();
 }
