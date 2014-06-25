#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "animationThread.h"
#include <iostream>
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainWindow)
{
    ui->setupUi(this);
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    createLanguageMenu();
    loadLanguage("de");
    ui->tabWidget->setCurrentIndex(0);
    m_tabPosition = 0;
    guiSetup();
    drawPicture();
    drawAnimatedPicture();
}

MainWindow::~MainWindow()
{
    delete[] pixArray;
    delete scene;
    delete scene2;
    delete scene3;
    delete ui;
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
    scene = new QGraphicsScene(ui->graphicsView_4);
    ui->graphicsView_4->setScene(scene);

    aT1.setFPS(10);
    aT1.setGView(ui->graphicsView_4);
    aT1.setPixArray(pixArray);
    aT1.setScence(scene);
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
  IO m_ioFile = IO((char*)"sample_1.gif");
  m_ioFile.loadFile();
  m_picFromIO = m_ioFile.getGif();
  m_drawPicture = generatePixmapFromPicture(*m_picFromIO);
  displayPicture(ui->tab1_graphicsView1, m_drawPicture);
  displayHeaderInfo(ui->tab1_textEdit1, m_picFromIO);
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

QPixmap MainWindow::generatePixmapFromPicture(Picture &p_pic)
{
    QPixmap pixmap(p_pic.getWidth(), p_pic.getHeight());
    QPainter p(&pixmap);
    char *pixel = p_pic.getPixel();
    int counter = 0;

    QColor color;
    for (int i = 0; i < p_pic.getHeight(); i++) {
        for(int j = 0; j < p_pic.getWidth(); j++ ){

            qDebug() << IO::getBit((unsigned int)pixel[counter],0,8) << " " << IO::getBit((unsigned int)pixel[counter+1],0, 8) << " " <<IO::getBit((unsigned int)pixel[counter+2],0,8) << endl;
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

void MainWindow::displayPicture(QGraphicsView *view, QPixmap &pic)
{
    QGraphicsScene *scene = new QGraphicsScene(view);
    view->setScene(scene);

    scene->addPixmap(pic);
    view->repaint();
}

void MainWindow::displayHeaderInfo(QTextEdit *p_textEdit, Picture *p_picFromIO)
{
    Gif* headerInfo = static_cast<Gif*>(p_picFromIO);
    if(headerInfo != 0) {
        m_headerInfo = "";
        m_headerInfo.append("Width: ");
        m_headerInfo.append(QString("%1 px").arg(headerInfo->getWidth()));
        m_headerInfo.append("\n");
        m_headerInfo.append("Height: ");
        m_headerInfo.append(QString("%1 px").arg(headerInfo->getHeight()));
        m_headerInfo.append("\n");
        m_headerInfo.append("GCT Flag: ");
        m_headerInfo.append(QString("%1").arg(headerInfo->getGctFlag()));
        m_headerInfo.append("\n");
        m_headerInfo.append("GCT Size: ");
        m_headerInfo.append(QString("%1").arg(headerInfo->getSizeOfGCT()));
        m_headerInfo.append("\n");
        m_headerInfo.append("BG Color: ");
        m_headerInfo.append(QString("%1").arg(headerInfo->getBgColor()));
        m_headerInfo.append("\n");
        m_headerInfo.append("Frames: ");
        m_headerInfo.append(QString("%1").arg(headerInfo->getSizeOfImages()));
        m_headerInfo.append("\n");

        p_textEdit->setText(m_headerInfo);
    }


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
   default:
       break;
   }
}

void MainWindow::on_actionBeenden_triggered()
{
    exit(0);
}

void MainWindow::on_actionDatei_ffnen_triggered()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                                     tr("GIF (*.gif*);;PNG (*.png*);;TIFF (.tif)"));

    //m_ioFile = IO(fileName.toStdString());
    m_ioFile.loadFile();
    m_picFromIO = m_ioFile.getGif();
    m_drawPicture = generatePixmapFromPicture(*m_picFromIO);
    displayPicture(ui->tab1_graphicsView1, m_drawPicture);

    ui->tabWidget->setCurrentIndex(0);  //Displays first Tab
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
