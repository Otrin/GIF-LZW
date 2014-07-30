#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "animationThread.h"
#include "loadingworker.h"
#include "compressor.h"
#include "huffman.h"
#include "runlengthencoding.h"
#include <iostream>
#include <fstream>
#include <sstream>
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
    m_delaytimes = NULL;
    m_scene = NULL;
    m_loadThread = NULL;
    m_loadWorker = NULL;
    m_animPrepWorker = NULL;
    m_animPrepThread = NULL;
    m_loadThread = NULL;
    m_ioFile = NULL;
    m_rawData = NULL;
    m_loading = false;
    m_animated = false;
    m_lastFileWasRaw = false;
    m_rawDataSize = 0;
    m_tab1Prepared = false;
    m_tab2Prepared = false;
    m_tab3Prepared = false;
    m_aboutDialog = NULL;
    m_instructionDialog = NULL;
    createLanguageMenu();
    loadLanguage("de");
    guiSetup();
    ui->tab1_graphicsView_1->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->tabWidget->setCurrentIndex(0);
    setWindowIcon(QIcon("rsc/windowIcon.gif"));

    qApp->processEvents();
    setWindowTitle("GIF LZW Visualizer - GIF Modus");
}

MainWindow::~MainWindow()
{
    m_animationThread.stopAnim();
    if(m_loadThread != NULL) m_loadThread->exit(0);
    if(m_animPrepThread != NULL) m_animPrepThread->exit(0);

    if(m_loadWorker != NULL) delete m_loadWorker;
    if(m_loadThread != NULL) delete m_loadThread;

    if(m_picFromIO != NULL && m_animated){
        Gif* gif = static_cast<Gif*>(m_picFromIO);
        for (int i = 0; i < gif->getSizeOfFrames(); ++i) {
            delete m_animatedPicture[i];
            m_animatedPicture[i] = NULL;
        }
        delete[] m_delaytimes;
        delete m_animPrepWorker;
        delete m_animPrepThread;
    }
    if(m_loadWorker != NULL) delete m_loadWorker;
    if(m_loadThread != NULL) delete m_loadThread;
    if(m_ioFile != NULL) delete m_ioFile;
    if(m_rawData != NULL) delete[] m_rawData;

    delete ui;
    delete m_aboutDialog;
    delete m_instructionDialog;
}

void MainWindow::guiSetup()
{
    m_mode = GIF;
    loadFile(QString("rsc/startup.gif"));
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
        unsigned char *pixel = gif->getFrame(0)->getPixel();
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



bool MainWindow::loadFile(QString p_filePath){
    if(!m_loading){
        m_loading = true;
        m_animationThread.stopAnim();

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
            m_animatedPicture = NULL;
            m_animated = false;
            delete[] m_delaytimes;
            m_delaytimes = NULL;
        }

        if(m_ioFile != NULL){
            delete m_ioFile;
            m_ioFile = NULL;
        }

        if(m_lastFileWasRaw){
           if(m_rawData != NULL) delete[] m_rawData;
           m_lastFileWasRaw = false;
           m_rawData = NULL;
           m_rawDataSize = 0;
        }

        m_tab1Prepared = false;
        m_tab2Prepared = false;
        m_tab3Prepared = false;

        if(m_mode == GIF){  //Picture is a GIF
            m_loadThread = new QThread;
            m_loadWorker = new LoadingWorker(p_filePath);
            m_loadWorker->moveToThread(m_loadThread);
            connect(m_loadWorker, SIGNAL(picReady(Picture*)), this, SLOT(onPicReady(Picture*)));
            connect(m_loadWorker, SIGNAL(ioReady(IO*)), this, SLOT(onIOReady(IO*)));
            connect(m_loadWorker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
            connect(m_loadThread, SIGNAL(started()), m_loadWorker, SLOT(process()));
            connect(m_loadWorker, SIGNAL(finished()), m_loadThread, SLOT(quit()));

            if(m_currLang == "de")
                ui->statusBar->showMessage("Lade GIF...");
            if(m_currLang == "en")
                ui->statusBar->showMessage("Loading GIF...");
            m_loadThread->start();
            return true;
        }
        else
            if(m_mode == PICTURE){  // Non-GIF Picture
                if(m_currLang == "de")
                    ui->statusBar->showMessage("Lade Bild...");
                if(m_currLang == "en")
                    ui->statusBar->showMessage("Loading Picture...");
                m_qImgFromIO = QImage(p_filePath);
                m_stillPicture = QPixmap::fromImage(m_qImgFromIO);
                displayPicture(ui->tab1_graphicsView_1, m_stillPicture);
                displayHeaderInfo(ui->tab1_textEdit_1, m_qImgFromIO);
                if(m_currLang == "de")
                     ui->statusBar->showMessage(QString("Ladevorgang beendet - Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 3000);
                if(m_currLang == "en")
                     ui->statusBar->showMessage(QString("Loading done - Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 3000);
                m_loading = false;
                ui->tabWidget->setCurrentIndex(0);  //Displays first Tab
                return true;
            }
            else
                if(m_mode == RAW){//Raw Data
                    m_lastFileWasRaw = true;
                    if(m_currLang == "de")
                         ui->statusBar->showMessage(QString("Lade Rohdaten..."), 3000);
                    if(m_currLang == "en")
                         ui->statusBar->showMessage(QString("Loading raw Data..."), 3000);

                    const QByteArray asc = p_filePath.toLocal8Bit();
                    std::string fileName = std::string(asc.constData(), asc.length());

                    std::streampos fsize = 0;
                    std::ifstream file( fileName.c_str(), std::ios::binary );

                    fsize = file.tellg();
                    file.seekg( 0, std::ios::end );
                    fsize = file.tellg() - fsize;
                    file.seekg(0);
                    qDebug() << fsize;
                    m_rawDataSize = fsize;
                    m_rawData = new unsigned char[m_rawDataSize];
                    file.read((char*)m_rawData, m_rawDataSize);
                    file.close();

                    m_loading = false;
                    if(m_currLang == "de")
                         ui->statusBar->showMessage(QString("Ladevorgang beendet"), 3000);
                    if(m_currLang == "en")
                         ui->statusBar->showMessage(QString("Loading done"), 3000);
                    m_loading = false;
                    ui->tabWidget->setCurrentIndex(1);  //Displays first Tab
                    return true;
                }
    }
    return false;
}


void MainWindow::onPicReady(Picture *p_pic){
    m_picFromIO = p_pic;
    Gif* gif = static_cast<Gif*>(m_picFromIO);

    if(gif->getSizeOfFrames() == 1){        //GIF only has one Frame
        m_stillPicture = generatePixmapFromPicture(m_picFromIO);
        displayPicture(ui->tab1_graphicsView_1, m_stillPicture);
        displayHeaderInfo(ui->tab1_textEdit_1, ui->tab1_textEdit_2, m_picFromIO);
        ui->tabWidget->setCurrentIndex(0);  //Displays first Tab
        if(m_currLang == "de")
            ui->statusBar->showMessage(QString("Ladevorgang beendet - Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 3000);
        if(m_currLang == "en")
            ui->statusBar->showMessage(QString("Loading done - Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 3000);
        m_loading = false;
    } else
        if(gif->getSizeOfFrames() > 1 && checkDelayTime(gif)){ // Several Frames
            m_stillPicture = generatePixmapFromPicture(m_picFromIO);
            displayPicture(ui->tab1_graphicsView_1, m_stillPicture);
            displayHeaderInfo(ui->tab1_textEdit_1,ui->tab1_textEdit_2, m_picFromIO);
            ui->tabWidget->setCurrentIndex(0);  //Displays first Tab
            if(m_currLang == "de")
                ui->statusBar->showMessage(QString("Ladevorgang beendet - Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 3000);
            if(m_currLang == "en")
                ui->statusBar->showMessage(QString("Loading done - Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 3000);
            m_loading = false;
        } else { //animated GIF
            m_animPrepThread = new QThread;
            m_animPrepWorker = new AnimationPrepWorker(m_picFromIO);
            m_animPrepWorker->moveToThread(m_animPrepThread);
            connect(m_animPrepWorker, SIGNAL(pixArrayReady(QPixmap**)), this, SLOT(onPixArrayReady(QPixmap**)));
            connect(m_animPrepWorker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
            connect(m_animPrepThread, SIGNAL(started()),  m_animPrepWorker, SLOT(process()));
            connect( m_animPrepWorker, SIGNAL(finished()), m_animPrepThread, SLOT(quit()));
            connect(m_animPrepWorker, SIGNAL(finished()), m_animPrepWorker, SLOT(deleteLater()));
            connect(m_animPrepThread, SIGNAL(finished()), m_animPrepThread, SLOT(deleteLater()));
            if(m_currLang == "de")
                ui->statusBar->showMessage("Bereite Animation vor...");
            if(m_currLang == "en")
                ui->statusBar->showMessage("Preparing Animation...");
            m_animPrepThread->start();
        }
}

void MainWindow::onIOReady(IO *p_io)
{
    m_ioFile = p_io;
}


void MainWindow::onPixArrayReady(QPixmap **p_pixArray)
{
    m_animatedPicture = p_pixArray;
    Gif *gif = static_cast<Gif*>(m_picFromIO);
    m_delaytimes = generateDelayTimeArray(gif);
    m_animationThread.initPicture(gif, ui->tab1_graphicsView_1, m_animatedPicture, gif->getSizeOfFrames(), m_delaytimes);
    scalePicture(ui->tab1_graphicsView_1, ui->tab1_graphicsView_1->scene(), gif->getWidth());
    m_animationThread.startAnim();
    ui->tabWidget->setCurrentIndex(0);  //Displays first Tab
    displayHeaderInfo(ui->tab1_textEdit_1, ui->tab1_textEdit_2, m_picFromIO);
    m_animated = true;

    if(m_currLang == "de")
        ui->statusBar->showMessage(QString("Ladevorgang beendet - Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 3000);
    if(m_currLang == "en")
        ui->statusBar->showMessage(QString("Loading done - Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 3000);

    ui->tabWidget->setCurrentIndex(0);  //Displays first Tab
    m_loading = false;
}


bool MainWindow::checkDelayTime(Gif *p_gif)
{
    bool chk = true;

    for (int i = 0; i < p_gif->getSizeOfFrames(); i++) {
        if(p_gif->getFrame(i)->getDelayTime() != 0) chk = false;
    }

    return chk;
}

int *MainWindow::generateDelayTimeArray(Gif *p_gif)
{
    int *fps = new int[p_gif->getSizeOfFrames()];

    for (int i = 0; i < p_gif->getSizeOfFrames(); i++) {
        fps[i] = p_gif->getFrame(i)->getDelayTime();
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
        m_headerInfo.append(QString("Height: %1 px\n").arg(gif->getHeight()));
        double rawDataSize = 0.0;
        for (int i = 0; i < gif->getSizeOfFrames(); ++i) {
            rawDataSize += gif->getFrame(i)->getSizeOfData()/1000.0;
        }
        m_headerInfo.append(QString("Raw Data: %1 kB\n").arg(QString::number(rawDataSize, 'f', 2)));
        m_headerInfo.append(QString("Frames: %1\n\n").arg(gif->getSizeOfFrames()));

        m_headerInfo.append(QString("GCT Flag: %1\n").arg(gif->getGctFlag()));
        m_headerInfo.append(QString("GCT Size: %1\n").arg(gif->getSizeOfGCT()));
        m_headerInfo.append(QString("BG Color: %1\n").arg(gif->getBgColor()));
        m_headerInfo.append(QString("Interlace-Flag: %1\n").arg(gif->getFrame(0)->getInterlaceFlag()));
        m_headerInfo.append(QString("Text: %1\n").arg("")); //hier text anzeigen

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
            m_headerInfo.append(QString("Codetable Size: %1 \n\n").arg(gif->getFrame(i)->getSizeOfData()));
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
   case Qt::Key_Plus:
        if(event->modifiers() & Qt::ControlModifier){
            double scaleFactor = 1.30;
            switch (ui->tabWidget->currentIndex()) {
            case 0:
                ui->tab1_graphicsView_1->scale(scaleFactor, scaleFactor);
                ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 1000);
                break;
            case 1:
                ui->tab2_graphicsView_1->scale(scaleFactor, scaleFactor);
                ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab2_graphicsView_1->transform().m11(),'f',2)), 1000);
                break;
            case 2:
                ui->tab3_graphicsView_1->scale(scaleFactor, scaleFactor);
                ui->tab3_graphicsView_2->scale(scaleFactor, scaleFactor);
                ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab3_graphicsView_1->transform().m11(),'f',2)), 1000);
                break;
            default:
                break;
            }
        }
        break;
   case Qt::Key_Minus:
        if(event->modifiers() & Qt::ControlModifier){
           double scaleFactor = 1.30;
           switch (ui->tabWidget->currentIndex()) {
           case 0:
               ui->tab1_graphicsView_1->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
               ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 1000);
               break;
           case 1:
               ui->tab2_graphicsView_1->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
               ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab2_graphicsView_1->transform().m11(),'f',2)), 1000);
               break;
           case 2:
               ui->tab3_graphicsView_1->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
               ui->tab3_graphicsView_2->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
               ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab3_graphicsView_1->transform().m11(),'f',2)), 1000);
               break;
           default:
               break;
           }
        }
        break;
   case Qt::Key_0:
       if(event->modifiers() & Qt::ControlModifier){

           switch (ui->tabWidget->currentIndex()) {
           case 0:
               ui->tab1_graphicsView_1->setTransform(QTransform::fromScale(1.0, 1.0));
               ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 1000);
               break;
           case 1:
               ui->tab2_graphicsView_1->setTransform(QTransform::fromScale(1.0, 1.0));
               ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab2_graphicsView_1->transform().m11(),'f',2)), 1000);
               break;
           case 2:
               ui->tab3_graphicsView_1->setTransform(QTransform::fromScale(1.0, 1.0));
               ui->tab3_graphicsView_2->setTransform(QTransform::fromScale(1.0, 1.0));
               ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab3_graphicsView_1->transform().m11(),'f',2)), 1000);
               break;
           default:
               break;
           }
       }
       break;
   case Qt::Key_Period:
       if(event->modifiers() & Qt::ControlModifier){
           switch (ui->tabWidget->currentIndex()) {
           case 0:
               ui->tab1_graphicsView_1->fitInView(ui->tab1_graphicsView_1->scene()->sceneRect(), Qt::KeepAspectRatio);
               ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 1000);
               break;
           case 1:
               ui->tab2_graphicsView_1->fitInView(ui->tab2_graphicsView_1->scene()->sceneRect(), Qt::KeepAspectRatio);
               ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab2_graphicsView_1->transform().m11(),'f',2)), 1000);
               break;
           case 2:
                //Currently disabled because it seems to crash without a picture in both scenes
               //ui->tab3_graphicsView_1->fitInView(ui->tab3_graphicsView_1->scene()->sceneRect(), Qt::KeepAspectRatio);
               ui->tab3_graphicsView_2->fitInView(ui->tab3_graphicsView_2->scene()->sceneRect(), Qt::KeepAspectRatio);
               ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab3_graphicsView_1->transform().m11(),'f',2)), 1000);
               break;
           default:
               break;
           }
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
        switch (ui->tabWidget->currentIndex()) {
        case 0:
            ui->tab1_graphicsView_1->scale(scaleFactor, scaleFactor);
            ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 1000);
            break;
        case 1:
            ui->tab2_graphicsView_1->scale(scaleFactor, scaleFactor);
            ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab2_graphicsView_1->transform().m11(),'f',2)), 1000);
            break;
        case 2:
            ui->tab3_graphicsView_1->scale(scaleFactor, scaleFactor);
            ui->tab3_graphicsView_2->scale(scaleFactor, scaleFactor);
            ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab3_graphicsView_1->transform().m11(),'f',2)), 1000);
            break;
        default:
            break;
        }

    } else {
        // Zoom out
        switch (ui->tabWidget->currentIndex()) {
        case 0:
            ui->tab1_graphicsView_1->scale(1.0/scaleFactor, 1.0/scaleFactor);
            ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab1_graphicsView_1->transform().m11(),'f',2)), 1000);
            break;
        case 1:
            ui->tab2_graphicsView_1->scale(1.0/scaleFactor, 1.0/scaleFactor);
            ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab2_graphicsView_1->transform().m11(),'f',2)), 1000);
            break;
        case 2:
            ui->tab3_graphicsView_1->scale(1.0/scaleFactor, 1.0/scaleFactor);
            ui->tab3_graphicsView_2->scale(1.0/scaleFactor, 1.0/scaleFactor);
            ui->statusBar->showMessage(QString("Zoom: %1 x").arg(QString::number(ui->tab3_graphicsView_1->transform().m11(),'f',2)), 1000);
            break;
        default:
            break;
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event){
    m_animationThread.stopAnim();
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
    m_animationThread.stopAnim();
    exit(0);
}

void MainWindow::on_actionDatei_ffnen_triggered()
{
    QString fileName;
    if(m_currLang == "en")
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                                     tr("Image Files (*.gif *.png *.jpg *.jpeg *.bmp *.tiff);;Raw Data(*)"));
    else
        if((m_currLang == "de"))
            fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                                     tr("Bild Dateien (*.gif *.png *.jpg *.jpeg *.bmp *.tiff);;Rohdaten(*)"));
    if(fileName != NULL){
        if(fileName.endsWith("gif")){
            ui->tabWidget->setTabEnabled(0, true);
            ui->tabWidget->setTabEnabled(1, true);
            ui->tabWidget->setTabEnabled(2, true);
            ui->tabWidget->setTabEnabled(3, true);
            ui->tab2_graphicsView_1->setVisible(true);
            ui->tab2_graphicsView_2->setVisible(true);
            m_mode = GIF;
            if(m_currLang == "en")
                MainWindow::setWindowTitle("GIF LZW Visualizer - GIF Mode");
            else
                if((m_currLang == "de"))
                    MainWindow::setWindowTitle("GIF LZW Visualizer - GIF Modus");

            loadFile(fileName);
        }
        else
            if(fileName.endsWith("png") || fileName.endsWith("png") || fileName.endsWith("jpg") || fileName.endsWith("jpeg")
                    || fileName.endsWith("bmp") || fileName.endsWith("tiff")){
                m_mode = PICTURE;
                ui->tab2_graphicsView_1->setVisible(true);
                ui->tab2_graphicsView_2->setVisible(true);
                ui->tabWidget->setTabEnabled(0, true);
                ui->tabWidget->setTabEnabled(2, false);
                if(m_currLang == "en")
                    MainWindow::setWindowTitle("GIF LZW Visualizer - Picture Mode");
                else
                    if((m_currLang == "de"))
                        MainWindow::setWindowTitle("GIF LZW Visualizer - Bild Modus");

                loadFile(fileName);
            }
            else{
                m_mode = RAW;
                ui->tab2_graphicsView_1->setVisible(false);
                ui->tab2_graphicsView_2->setVisible(false);
                ui->tabWidget->setTabEnabled(0, false);
                ui->tabWidget->setTabEnabled(2, false);
                if(m_currLang == "en")
                    MainWindow::setWindowTitle("GIF LZW Visualizer - Raw Data Mode");
                else
                    if((m_currLang == "de"))
                        MainWindow::setWindowTitle("GIF LZW Visualizer - Rohdaten Modus");
                loadFile(fileName);
            }
   }
}

void MainWindow::on_actionGIF_Bild_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                     tr("GIF (*.gif*)"));

    if(!fileName.contains(".gif")) fileName.append(".gif");
    qDebug() << fileName;
    IO gifIOFile(fileName.toStdString());
    Gif gif;
    Gif *gif_ = static_cast<Gif*>(m_picFromIO);
    gif.setHeight(gif_->getHeight());
    gif.setWidth(gif_->getWidth());
    gif.setPixel(gif_->getFrame(0)->getPixel());
    gifIOFile.setGif(gif);
    gifIOFile.generateFile();
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

void MainWindow::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0:
        initTab0();
        break;
    case 1:
        initTab1();
        break;
    case 2:
        initTab2();
        break;
    case 3:
        initTab3();
        break;
    default:
        break;
    }
}

void MainWindow::changeAnimGView(QGraphicsView *p_gView)
{
        m_animationThread.stopAnim();
        m_animationThread.setGView(p_gView);
        m_animationThread.resetScence();
        m_animationThread.generateGItemPointer();
        m_animationThread.resetCounter();
        m_animationThread.startAnim();
}


void MainWindow::on_action_ber_triggered()
{
    if(m_aboutDialog == NULL){
        m_aboutDialog = new AboutDialog;
    }

    m_aboutDialog->show();
}

void MainWindow::on_actionAnleitung_triggered()
{
    if(m_instructionDialog == NULL){
        m_instructionDialog = new InstructionDialog;
    }

    m_instructionDialog->show();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    foreach(QUrl url, event->mimeData()->urls()){
//        if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="GIF"){
//            event->acceptProposedAction();
//            return;
//        } else
//            if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="JPG"){
//                event->acceptProposedAction();
//                return;
//            } else
//                if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="JPEG"){
//                    event->acceptProposedAction();
//                    return;
//                } else
//                    if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="PNG"){
//                        event->acceptProposedAction();
//                        return;
//                    } else
//                        if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="BMP"){
//                            event->acceptProposedAction();
//                            return;
//                        } else
//                            if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="TIFF"){
//                                event->acceptProposedAction();
//                                return;
//                            }

    //At First only Pictures where accepted by Drag&Drop -> now every File
        if(QFileInfo(url.toLocalFile()).isFile()){
            event->acceptProposedAction();
            return;
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event)
 {
     foreach(QUrl url, event->mimeData()->urls()){
         QString fileName = url.toLocalFile();
         if(fileName != NULL){
             if(fileName.endsWith("gif")){
                 ui->tabWidget->setTabEnabled(0, true);
                 ui->tabWidget->setTabEnabled(1, true);
                 ui->tabWidget->setTabEnabled(2, true);
                 ui->tabWidget->setTabEnabled(3, true);
                 ui->tab2_graphicsView_1->setVisible(true);
                 ui->tab2_graphicsView_2->setVisible(true);
                 m_mode = GIF;
                 if(m_currLang == "en")
                     MainWindow::setWindowTitle("GIF LZW Visualizer - GIF Mode");
                 else
                     if((m_currLang == "de"))
                         MainWindow::setWindowTitle("GIF LZW Visualizer - GIF Modus");

                 loadFile(fileName);
             }
             else
                 if(fileName.endsWith("png") || fileName.endsWith("png") || fileName.endsWith("jpg") || fileName.endsWith("jpeg")
                         || fileName.endsWith("bmp") || fileName.endsWith("tiff")){
                     m_mode = PICTURE;
                     ui->tab2_graphicsView_1->setVisible(true);
                     ui->tab2_graphicsView_2->setVisible(true);
                     ui->tabWidget->setTabEnabled(0, true);
                     ui->tabWidget->setTabEnabled(2, false);
                     if(m_currLang == "en")
                         MainWindow::setWindowTitle("GIF LZW Visualizer - Picture Mode");
                     else
                         if((m_currLang == "de"))
                             MainWindow::setWindowTitle("GIF LZW Visualizer - Bild Modus");

                     loadFile(fileName);
                 }
                 else{
                     m_mode = RAW;
                     ui->tab2_graphicsView_1->setVisible(false);
                     ui->tab2_graphicsView_2->setVisible(false);
                     ui->tabWidget->setTabEnabled(0, false);
                     ui->tabWidget->setTabEnabled(2, false);
                     if(m_currLang == "en")
                         MainWindow::setWindowTitle("GIF LZW Visualizer - Raw Data Mode");
                     else
                         if((m_currLang == "de"))
                             MainWindow::setWindowTitle("GIF LZW Visualizer - Rohdaten Modus");
                     loadFile(fileName);
                 }
        }
        return;  //only one file accepted
     }
}

void MainWindow::initTab0()
{
    if(!m_loading){
        if(m_animated){
            changeAnimGView(ui->tab1_graphicsView_1);
            scalePicture(ui->tab1_graphicsView_1, ui->tab1_graphicsView_1->scene(), m_picFromIO->getWidth());
        }
        else
            displayPicture(ui->tab1_graphicsView_1, m_stillPicture);
    }
}

void MainWindow::initTab1()
{
    if(!m_loading){
        if(m_animated){
            changeAnimGView(ui->tab2_graphicsView_1);
            scalePicture(ui->tab2_graphicsView_1, ui->tab2_graphicsView_1->scene(), m_picFromIO->getWidth());
        }
        else{
            displayPicture(ui->tab2_graphicsView_1, m_stillPicture);
        }
    }

    if(!m_tab1Prepared){
        // PATRICK CODE GOES HERE. FEEL FREE TO CHANGE THE ABOVE CODE IN THIS METHOD IF YOU NEED TO
        // THIS METHOD IS CALLED EVERY TIME THE CORRESPONDING TAB GETS FOCUS
        m_tab1Prepared = true;
    }
}

void MainWindow::initTab2()
{
    if(!m_loading){
        Gif* gif = static_cast<Gif*>(m_picFromIO);
        if(gif->getGctFlag()){
            if(m_animated){
                changeAnimGView(ui->tab3_graphicsView_2);
                scalePicture(ui->tab3_graphicsView_2, ui->tab3_graphicsView_2->scene(), m_picFromIO->getWidth());
            }
            else{
                displayPicture(ui->tab3_graphicsView_2, m_stillPicture);
            }
        } else {
            if(m_animated){
                changeAnimGView(ui->tab3_graphicsView_1);
                scalePicture(ui->tab3_graphicsView_1, ui->tab3_graphicsView_1->scene(), m_picFromIO->getWidth());
            }
            else{
                displayPicture(ui->tab3_graphicsView_1, m_stillPicture);
            }
        }
    }

    if(!m_tab2Prepared){
        // JOHANNES CODE GOES HERE. FEEL FREE TO CHANGE THE ABOVE CODE IN THIS METHOD IF YOU NEED TO
        // THIS METHOD IS CALLED EVERY TIME THE CORRESPONDING TAB GETS FOCUS
        m_tab2Prepared = true;
    }
}

void MainWindow::initTab3()
{
    if(!m_tab3Prepared){
        Compressor *compressor;
        unsigned char *rawData = m_ioFile->getGif()->getFrame(0)->getPixel(), *compData = NULL;
        int sizeOfRawData = m_ioFile->getGif()->getFrame(0)->getSizeOfPixel(), sizeOfCompData = 0;

        compressor = new RunLengthEncoding();
        compData = compressor->encode(rawData, sizeOfRawData, 0);
        sizeOfCompData = compressor->getSizeOfCompData();
        stringstream output;
        output << "\nKomprimieren: \n\n";
        output << "Kompressionsrate: " << ((double)sizeOfRawData/(double)sizeOfCompData);
        long time = compressor->getTimeAgo(), s, ms, mcs;
        time /=1000;
        mcs = time % 1000;
        time /= 1000;
        ms = time%1000;
        time /= 1000;
        s = time%1000;
        output << "\nAusführungszeit: " << s << "s " << ms << "ms " << mcs << "µs";


        output << "\n\n===========================";

        rawData = compressor->decode(compData, sizeOfCompData, NULL, 0);
        output << "\n\nDekomprimieren:\n\n";
        output << "Kompressionsrate: " << ((double)sizeOfRawData/(double)sizeOfCompData);
        time = compressor->getTimeAgo();
        time /=1000;
        mcs = time % 1000;
        time /= 1000;
        ms = time%1000;
        time /= 1000;
        s = time%1000;
        output << "\nAusführungszeit: " << s << "s " << ms << "ms " << mcs << "µs";
        QString text = QString::fromStdString(output.str());
        ui->tab4_textEdit_3->setText(text);

        unsigned char rd[]{'a', 'a', 'a', 'a', 'a', 'b', 'b', 'c', 'c', 'c', 'd', 'd', 'e', 'e', 'f', 'g', 'G', 'h'};
        rawData = m_ioFile->getGif()->getFrame(0)->getPixel();
        sizeOfRawData = m_ioFile->getGif()->getFrame(0)->getSizeOfPixel();

        compressor = new Huffman();
        compData = compressor->encode(rawData, sizeOfRawData, 0);
        unsigned char *codeTable = compressor->getCodeTable();
        int sizeOfCodeTable = compressor->getSizeOfCodeTable();
        sizeOfCompData = compressor->getSizeOfCompData();
        output.str("");
        output << "\nKomprimieren: \n\n";
        output << "Kompressionsrate: " << ((double)sizeOfRawData*8/(double)sizeOfCompData);
        time = compressor->getTimeAgo(), s, ms, mcs;
        time /=1000;
        mcs = time % 1000;
        time /= 1000;
        ms = time%1000;
        time /= 1000;
        s = time%1000;
        output << "\nAusführungszeit: " << s << "s " << ms << "ms " << mcs << "µs";


        output << "\n\n===========================";

        rawData = compressor->decode(compData, sizeOfCompData, codeTable, sizeOfCodeTable);
        output << "\n\nDekomprimieren:\n\n";
        output << "Kompressionsrate: " << ((double)sizeOfRawData*8/(double)sizeOfCompData);
        time = compressor->getTimeAgo();
        time /=1000;
        mcs = time % 1000;
        time /= 1000;
        ms = time%1000;
        time /= 1000;
        s = time%1000;
        output << "\nAusführungszeit: " << s << "s " << ms << "ms " << mcs << "µs";
        text = QString::fromStdString(output.str());
        ui->tab4_textEdit_2->setText(text);

//        // ERIK CODE GOES HERE
//        // THIS METHOD IS CALLED EVERY TIME THE CORRESPONDING TAB GETS FOCUS
//        m_tab3Prepared = true;
    }
}
