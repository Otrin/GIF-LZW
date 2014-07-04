#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "animationThread.h"
#include "loadingworker.h"
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
    m_animatedPicture = NULL;
    m_picFromIO = NULL;
    m_fps = NULL;
    m_scene = NULL;
    m_loadThread = NULL;
    m_loadWorker = NULL;
    m_animPrepWorker = NULL;
    m_animPrepThread = NULL;
    m_loadThread = NULL;
    m_animated = false;
    createLanguageMenu();
    loadLanguage("de");
    guiSetup();
    ui->tab1_graphicsView_1->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->tabWidget->setCurrentIndex(0);
    m_tabPosition = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_aboutDialog;
    delete m_instructionDialog;
}

void MainWindow::guiSetup()
{
    if(loadPicture(QString("rsc/startup.gif"))){
        displayPicture(ui->tab1_graphicsView_1, m_drawPicture);
        displayHeaderInfo(ui->tab1_textEdit_1, ui->tab1_textEdit_2, m_picFromIO);
    }
}

void MainWindow::dropSetup(){
    ui->tab1_graphicsView_1->setAcceptDrops(false);
    ui->tab1_textEdit_1->setAcceptDrops(false);
    ui->tab1_textEdit_2->setAcceptDrops(false);
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
        QPixmap pixmap(gif->getFrame(0)->getWidth(), gif->getFrame(0)->getHeight());
        QPainter p(&pixmap);
        char *pixel = gif->getFrame(0)->getPixel();
        int counter = 0;

        QColor color;
        for (int i = 0; i < gif->getFrame(0)->getHeight(); i++) {
            for(int j = 0; j < gif->getFrame(0)->getWidth(); j++ ){
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
    m_animThreadGView1.stopAnim();
    if(m_loadThread != NULL) m_loadThread->exit(0);
    if(m_animPrepThread != NULL) m_animPrepThread->exit(0);

    if(m_loadWorker != NULL){
        delete m_loadWorker;
        m_loadWorker = NULL;
    }
    if(m_loadThread != NULL){
        delete m_loadThread;
        m_loadThread = NULL;
    }

    if(m_picFromIO != NULL && m_animated){
        Gif* gif = static_cast<Gif*>(m_picFromIO);
        for (int i = 0; i < gif->getSizeOfFrames(); ++i) {
            delete m_animatedPicture[i];
            m_animatedPicture[i] = NULL;
        }
//        delete[] m_animatedPicture;
//        m_animatedPicture = NULL;
        m_animated = false;
        delete[] m_fps;
        m_fps = 0;
        delete m_animPrepWorker;
        m_animPrepWorker = NULL;
        delete m_animPrepThread;
        m_animPrepThread = NULL;
    }

    if(p_filePath.endsWith(".gif")){  //Picture is a GIF
        if(m_loadWorker != NULL) delete m_loadWorker;
        m_loadWorker = NULL;
        if(m_loadThread != NULL) delete m_loadThread;
        m_loadThread = NULL;

        m_loadThread = new QThread;
        m_loadWorker = new LoadingWorker(p_filePath);
        m_loadWorker->moveToThread(m_loadThread);
        connect(m_loadWorker, SIGNAL(picReady(Picture*)), this, SLOT(onPicReady(Picture*)));
        connect(m_loadWorker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(m_loadThread, SIGNAL(started()), m_loadWorker, SLOT(process()));
        connect(m_loadWorker, SIGNAL(finished()), m_loadThread, SLOT(quit()));
        if(m_currLang == "de")
            ui->statusBar->showMessage("Lade GIF...");
        if(m_currLang == "en")
            ui->statusBar->showMessage("Loading GIF...");
        m_loadThread->start();
    }
    else{           //Picture is NOT a GIF
        if(m_currLang == "de")
            ui->statusBar->showMessage("Lade Bild...");
        if(m_currLang == "en")
            ui->statusBar->showMessage("Loading Picture...");
        m_qImgFromIO = QImage(p_filePath);
        m_drawPicture = QPixmap::fromImage(m_qImgFromIO);
        displayPicture(ui->tab1_graphicsView_1, m_drawPicture);
        displayHeaderInfo(ui->tab1_textEdit_1, m_qImgFromIO);
        if(m_currLang == "de")
            ui->statusBar->showMessage("Ladevorgang fertig", 1000);
        if(m_currLang == "en")
            ui->statusBar->showMessage("Loading done", 1000);
        return true;
    }
    return false;
}


void MainWindow::onPicReady(Picture *p_pic){
    m_picFromIO = p_pic;
    Gif* gif = static_cast<Gif*>(m_picFromIO);

    if(gif->getSizeOfFrames() == 1){        //GIF only has one Frame
        m_drawPicture = generatePixmapFromPicture(m_picFromIO);
        displayPicture(ui->tab1_graphicsView_1, m_drawPicture);
        displayHeaderInfo(ui->tab1_textEdit_1, ui->tab1_textEdit_2, m_picFromIO);
        ui->tabWidget->setCurrentIndex(0);  //Displays first Tab
        if(m_currLang == "de")
            ui->statusBar->showMessage("Ladevorgang fertig", 1000);
        if(m_currLang == "en")
            ui->statusBar->showMessage("Loading done", 1000);
    } else
        if(gif->getSizeOfFrames() > 1 && checkDelayTime(gif)){ // Several Frames
            m_drawPicture = generatePixmapFromPicture(m_picFromIO);
            displayPicture(ui->tab1_graphicsView_1, m_drawPicture);
            displayHeaderInfo(ui->tab1_textEdit_1,ui->tab1_textEdit_2, m_picFromIO);
            ui->tabWidget->setCurrentIndex(0);  //Displays first Tab
            if(m_currLang == "de")
                ui->statusBar->showMessage("Ladevorgang fertig", 1000);
            if(m_currLang == "en")
                ui->statusBar->showMessage("Loading done", 1000);
        } else { //animated GIF
            m_animPrepThread = new QThread;
            m_animPrepWorker = new AnimationPrepWorker(m_picFromIO);
            m_animPrepWorker->moveToThread(m_animPrepThread);
            connect(m_animPrepWorker, SIGNAL(pixArrayReady(QPixmap**)), this, SLOT(onPixArrayReady(QPixmap**)));
            connect(m_animPrepWorker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
            connect(m_animPrepThread, SIGNAL(started()),  m_animPrepWorker, SLOT(process()));
            connect( m_animPrepWorker, SIGNAL(finished()), m_animPrepThread, SLOT(quit()));
            if(m_currLang == "de")
                ui->statusBar->showMessage("Bereite Animation vor...");
            if(m_currLang == "en")
                ui->statusBar->showMessage("Preparing Animation...");
            m_animPrepThread->start();
        }
}


void MainWindow::onPixArrayReady(QPixmap **p_pixArray)
{
    m_animatedPicture = p_pixArray;
    Gif *gif = static_cast<Gif*>(m_picFromIO);
    m_fps = generateDelayTimeArray(gif);
    m_animThreadGView1.initPicture(gif, ui->tab1_graphicsView_1, m_animatedPicture, gif->getSizeOfFrames(), m_fps);
    scalePicture(ui->tab1_graphicsView_1, ui->tab1_graphicsView_1->scene(), gif->getWidth());
    m_animThreadGView1.startAnim();
    ui->tabWidget->setCurrentIndex(0);  //Displays first Tab
    displayHeaderInfo(ui->tab1_textEdit_1, ui->tab1_textEdit_2, m_picFromIO);
    m_animated = true;
    ui->tabWidget->setCurrentIndex(0);  //Displays first Tab
    if(m_currLang == "de")
        ui->statusBar->showMessage("Ladevorgang fertig", 1000);
    if(m_currLang == "en")
        ui->statusBar->showMessage("Loading done", 1000);
}


bool MainWindow::checkDelayTime(Gif *gif)
{
    bool chk = true;

    for (int i = 0; i < gif->getSizeOfFrames(); i++) {
        if(gif->getFrame(i)->getDelayTime() != 0) chk = false;
    }

    return chk;
}

int *MainWindow::generateDelayTimeArray(Gif *gif)
{
    int *fps = new int[gif->getSizeOfFrames()];

    for (int i = 0; i < gif->getSizeOfFrames(); i++) {
        fps[i] = gif->getFrame(i)->getDelayTime();
    }

    return fps;
}


void MainWindow::displayPicture(QGraphicsView *view, QPixmap &pic)
{
    delete m_scene;
    m_scene = new QGraphicsScene(view);
    view->setScene(m_scene);

    m_scene->addPixmap(pic);
    view->repaint();

    scalePicture(view, m_scene, pic.width());
}

void MainWindow::displayHeaderInfo(QTextEdit *p_textEdit, QTextEdit *p_textEdit2, Picture *p_picFromIO)
{
    Gif* gif = static_cast<Gif*>(p_picFromIO);
    if(gif != 0) {
        m_headerInfo = "";
        m_headerInfo.append(QString("Width: %1 px\n").arg(gif->getWidth()));
        m_headerInfo.append(QString("Height: %1 px\n\n").arg(gif->getHeight()));

        m_headerInfo.append(QString("GCT Flag: %1\n").arg(gif->getGctFlag()));
        m_headerInfo.append(QString("GCT Size: %1\n").arg(gif->getSizeOfGCT()));
        m_headerInfo.append(QString("BG Color: %1\n").arg(gif->getBgColor()));
        m_headerInfo.append(QString("Interlace-Flag: %1\n\n").arg(gif->getFrame(0)->getInterlaceFlag()));

        m_headerInfo.append(QString("Frames: %1\n\n").arg(gif->getSizeOfFrames()));

        p_textEdit->setText(m_headerInfo);


        m_headerInfo = "";
        for (int i = 0; i < gif->getSizeOfFrames(); ++i) {
            m_headerInfo.append(QString("Frame %1 \n").arg(i));
            m_headerInfo.append(QString("Width: %1 px\n").arg(gif->getFrame(i)->getWidth()));
            m_headerInfo.append(QString("Height: %1 px\n").arg(gif->getFrame(i)->getHeight()));
            m_headerInfo.append(QString("Top: %1 px\n").arg(gif->getFrame(i)->getTop()));
            m_headerInfo.append(QString("Left: %1 px\n").arg(gif->getFrame(i)->getLeft()));
            m_headerInfo.append(QString("Interlace Flag: %1 \n").arg(gif->getFrame(i)->getInterlaceFlag()));
            m_headerInfo.append(QString("DelayTime: %1 \n").arg(gif->getFrame(i)->getDelayTime()));
            m_headerInfo.append(QString("Transp. Color Flag: %1 \n").arg(gif->getFrame(i)->getTranspColorFlag()));
            m_headerInfo.append(QString("Transp. Color Index: %1 \n").arg(gif->getFrame(i)->getTranspColorIndex()));
            m_headerInfo.append(QString("LCT Flag: %1 \n").arg(gif->getFrame(i)->getLctFlag()));
            m_headerInfo.append(QString("LCT Size: %1 \n").arg(gif->getFrame(i)->getSizeOfLCT()));
            m_headerInfo.append(QString("Sort Flag: %1 \n").arg(gif->getFrame(i)->getSortFlag()));
            m_headerInfo.append(QString("Userinput Flag: %1 \n").arg(gif->getFrame(i)->getUserInputFlag()));
            m_headerInfo.append(QString("Disposal Method: %1 \n").arg(gif->getFrame(i)->getDisposualMethod()));
            m_headerInfo.append(QString("MinCodeSize: %1 \n").arg(gif->getFrame(i)->getMinCodeSize()));
            m_headerInfo.append(QString("Codetable Size: %1 \n\n").arg(gif->getFrame(i)->getSizeOfCodeTable()));
        }
        p_textEdit2->setText(m_headerInfo);
    }
}

void MainWindow::displayHeaderInfo(QTextEdit *p_textEdit, QImage &p_qImgFromIO)
{
    m_headerInfo = "";
    m_headerInfo.append(QString("Width: %1 px\n").arg(p_qImgFromIO.width()));
    m_headerInfo.append(QString("Height: %1 px\n\n").arg(p_qImgFromIO.height()));

    m_headerInfo.append(QString("Colortable Size: %1\n").arg(p_qImgFromIO.colorCount()));
    m_headerInfo.append(QString("Depth: %1\n").arg(p_qImgFromIO.depth()));
    m_headerInfo.append(QString("Alphachannel: %1\n").arg(p_qImgFromIO.hasAlphaChannel()));
    m_headerInfo.append(QString("Text: %1\n").arg(p_qImgFromIO.text()));


    p_textEdit->setText(m_headerInfo);
    ui->tab1_textEdit_2->setText("");
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


void MainWindow::repaint(QGraphicsView *p_gView)
{
    p_gView->repaint();
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
    m_animThreadGView1.stopAnim();
    event->accept();
    exit(0);
}

void MainWindow::scalePicture(QGraphicsView *p_view, QGraphicsScene *p_scene, int p_pictureWidth)
{
    p_view->setTransform(QTransform::fromScale(1.0, 1.0)); //reset scale

    if(p_pictureWidth > p_view->rect().width())
        p_view->fitInView(p_scene->sceneRect(), Qt::KeepAspectRatio);    //Zooms Picture to fit the View
    else
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

}

void MainWindow::on_actionBeenden_triggered()
{
    m_animThreadGView1.stopAnim();
    exit(0);
}

void MainWindow::on_actionDatei_ffnen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                                     tr("Image Files (*.gif *.png *.jpg *.jpeg *.bmp *.tiff)"));
    if(fileName != NULL) loadPicture(fileName);
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
         loadPicture(url.toLocalFile());
         return;  //only one file accepted
     }

     event->acceptProposedAction();
 }
