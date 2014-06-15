#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "animationThread.h"
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainWindow)
{
    ui->setupUi(this);
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    createLanguageMenu();
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
  //m_ioFile = IO("1.gif");
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
    for (int i = 0; i < p_pic.getHeight(); i++) {
        for(int j = 0; j < p_pic.getWidth(); j++ ){
            p.setPen(QColor((int)pixel[counter++], (int)pixel[counter++], (int)pixel[counter++]));
            p.drawPoint(i, j);
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
