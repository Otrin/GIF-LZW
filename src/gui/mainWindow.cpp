#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "animationThread.h"
#include "loadingworker.h"
#include "tableconversionworker.h"
#include "compressor.h"
#include "huffman.h"
#include "runlengthencoding.h"
#include "compressorworker.h"
#include <iostream>
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QUrl>
#include <QMimeData>
#include <QMessageBox>
#include <vector>
#include <QProgressBar>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::mainWindow)
{
	ui->setupUi(this);
	dropSetup();
	setAcceptDrops(true);
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	m_animatedPicture = NULL;
	m_animatedPicture2 = NULL;
	m_picFromIO = NULL;
	m_comparisonGif = NULL;
	m_delaytimes = NULL;
	m_delaytimes2 = NULL;
	m_loadThread = NULL;
	m_loadWorker = NULL;
    m_saveThread = NULL;
    m_saveWorker = NULL;
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
    m_tab2Calculating = false;
    m_tab3Calculating = false;
	m_aboutDialog = NULL;
   	m_instructionDialog = NULL;
    m_progressDialog = NULL;
	createLanguageMenu();
	loadLanguage("de");
	guiSetup();

    //Prepare QTableWidget
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(1);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->QAbstractItemView::scrollToBottom();
    connect( ui->tableWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( cellSelected() ) );
    QStringList tableHeader;
    tableHeader<<"Sequenz";
    ui->tableWidget->setHorizontalHeaderLabels(tableHeader);

	ui->tab1_graphicsView_1->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	ui->tabWidget->setCurrentIndex(0);
	m_tabPosition = 0;
	setWindowIcon(QIcon("rsc/windowIcon.gif"));
	
	qApp->processEvents();
    	setWindowTitle("GIF LZW Visualizer - GIF Modus");
}

MainWindow::~MainWindow()
{
	m_animationThread.stopAnim();
	m_animationThread2.stopAnim();
	if(m_loadThread != NULL) m_loadThread->exit(0);
    if(m_saveThread != NULL) m_saveThread->exit(0);
	if(m_animPrepThread != NULL) m_animPrepThread->exit(0);

	if(m_loadWorker != NULL) delete m_loadWorker;
	if(m_loadThread != NULL) delete m_loadThread;

	if(m_picFromIO != NULL && m_animated){
		Gif* gif = static_cast<Gif*>(m_picFromIO);
		for (int i = 0; i < gif->getSizeOfFrames(); ++i) {
			delete m_animatedPicture[i];
			m_animatedPicture[i] = NULL;
			if(m_tab2Prepared){
				delete m_animatedPicture2[i];
				m_animatedPicture2[i] = NULL;
			}
		}
		delete[] m_delaytimes;
		delete[] m_delaytimes2;
		delete m_animPrepWorker;
		delete m_animPrepThread;
	}
    if(m_loadWorker != NULL) delete m_saveWorker;
    if(m_loadThread != NULL) delete m_saveThread;
    if(m_saveWorker != NULL) delete m_saveWorker;
    if(m_saveThread != NULL) delete m_saveThread;
	if(m_ioFile != NULL) delete m_ioFile;
	if(m_rawData != NULL) delete[] m_rawData;

	delete ui;
	delete m_aboutDialog;
	delete m_instructionDialog;
    delete m_progressDialog;
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

void MainWindow::displayErrorMsg()
{
    QMessageBox msgBox;
    if(m_loading){
        if(m_currLang == "de")
            msgBox.setText("Bitte warten, bis das Laden der aktuellen Datei beendet ist.");
        else
            if(m_currLang == "en")
                 msgBox.setText("Please wait until the currently loading File is finished");
        }
    else
        if(m_tab2Calculating){
            if(m_currLang == "de")
                msgBox.setText("Bitte warten, bis der dritte Tab seine Berechnungen durchgeführt hat");
            else
                if(m_currLang == "en")
                     msgBox.setText("Please wait until the third Tab has finished its Calculations");
            }
        else
            if (m_tab3Calculating) {
                if(m_currLang == "de")
                    msgBox.setText("Bitte warten, bis der vierte Tab seine Berechnungen durchgeführt hat");
                else
                    if(m_currLang == "en")
                         msgBox.setText("Please wait until the fourth Tab has finished its Calculations");
                }
    msgBox.exec();
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

		if(gif->getSizeOfFrames() > 1){ //static pic with multiple small frames
			QPixmap pixmap(gif->getWidth(), gif->getHeight());
			QPainter p(&pixmap);

			QColor color;
			for (int k = 0; k < gif->getSizeOfFrames(); ++k) {
				int counter = 0;
				for (int i = 0; i < gif->getFrame(k)->getHeight(); ++i) {
					for(int j = 0; j < gif->getFrame(k)->getWidth(); ++j){
						color = QColor(IO::getBit((unsigned int)gif->getFrame(k)->getPixel()[counter],0,8),
									   IO::getBit((unsigned int)gif->getFrame(k)->getPixel()[counter+1],0,8),
								IO::getBit((unsigned int)gif->getFrame(k)->getPixel()[counter+2],0,8));
						p.setPen(color);
						p.drawPoint(gif->getFrame(k)->getLeft()+j, gif->getFrame(k)->getTop()+i);
						counter += 3;
					}

				}
			}
			return pixmap;

		}else{
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

	}
	return backUp;
}



bool MainWindow::loadFile(QString p_filePath){
    if(!m_loading && !m_tab2Calculating && !m_tab3Calculating){

		for (int i = 0; i < m_scenes.values().size(); ++i) {
			delete m_scenes.values()[i];
		}
		m_scenes.clear();

		m_scenes.insert(0, new QGraphicsScene(this));

		m_loading = true;
		m_animationThread.stopAnim();
		m_animationThread2.stopAnim();

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
				if(m_tab2Prepared){
					delete m_animatedPicture2[i];
					m_animatedPicture2[i] = NULL;
				}
			}
			m_animatedPicture = NULL;
			m_animated = false;
			delete[] m_delaytimes;
			delete[] m_delaytimes2;
			m_delaytimes = NULL;
			m_delaytimes2 = NULL;
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
				displayPicture(ui->tab1_graphicsView_1, m_stillPicture, 0);
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
    else{
       displayErrorMsg();
    }
    return false;
}


void MainWindow::onPicReady(Picture *p_pic){
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    m_encodingVisual.resetInternalState();
    m_picFromIO = p_pic;
	Gif* gif = static_cast<Gif*>(m_picFromIO);

	if(gif->getSizeOfFrames() == 1){        //GIF only has one Frame
		m_stillPicture = generatePixmapFromPicture(m_picFromIO);
		displayPicture(ui->tab1_graphicsView_1, m_stillPicture, 0);
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
			displayPicture(ui->tab1_graphicsView_1, m_stillPicture, 0);
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

void MainWindow::onSavingDone()
{
    m_progressDialog->close();
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


int MainWindow::displayPicture(QGraphicsView *view, QPixmap &pic, int p_sceneIndex)
{
	if(!m_scenes.contains(p_sceneIndex)){
		m_scenes.insert(p_sceneIndex,new QGraphicsScene(this));
	}
	else{
		m_scenes[p_sceneIndex]->clear();
	}

	view->setScene(m_scenes[p_sceneIndex]);

	m_scenes[p_sceneIndex]->addPixmap(pic);
	view->repaint();

	scalePicture(view, m_scenes[p_sceneIndex], pic.width());

	return p_sceneIndex;
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
	m_animationThread2.stopAnim();
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
	m_animationThread2.stopAnim();
	exit(0);
}

void MainWindow::on_actionDatei_ffnen_triggered()
{
    if(!m_loading && !m_tab2Calculating && !m_tab3Calculating){
        QString fileName;

        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                                tr("Image Files (*.gif *.png *.jpg *.jpeg *.bmp *.tiff);;Raw Data(*)"));

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
                if(fileName.endsWith("png") || fileName.endsWith("jpg") || fileName.endsWith("jpeg")
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
                        if(m_currLang == "de")
                            MainWindow::setWindowTitle("GIF LZW Visualizer - Rohdaten Modus");
                    loadFile(fileName);
                }
       }
    }
    else{
        displayErrorMsg();
    }
}

void MainWindow::on_actionGIF_Bild_triggered()
{
    if(!m_loading && !m_tab2Calculating && !m_tab3Calculating){
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                         tr("GIF (*.gif*)"));
        if(fileName != NULL){
            if(!fileName.contains(".gif")) fileName.append(".gif");

            if(m_progressDialog != NULL) delete m_progressDialog;
            QString labelText;
            QString buttonText;
            if(m_currLang == "en"){
                labelText = QString("Saving...");
            }
            else
                if(m_currLang == "de"){
                    labelText = QString("Speichervorgang...");
                }

            m_progressDialog = new QProgressDialog(labelText, buttonText, 0, 100);
            m_progressDialog->setModal(true);
            m_progressDialog->setFixedWidth(200);
            m_progressDialog->setFixedHeight(100);
            m_progressDialog->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            m_progressDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
            m_progressDialog->setCancelButton(0);


            QProgressBar* bar = new QProgressBar(m_progressDialog);
            bar->setRange(0, 0);
            bar->setValue(0);
            m_progressDialog->setBar(bar);

            m_saveThread = new QThread;


            switch(m_mode){
            case GIF:
                m_saveWorker = new SavingWorker(fileName, 0);
                m_saveWorker->setPicFromIO(m_picFromIO);
                break;
            case PICTURE:
                m_saveWorker = new SavingWorker(fileName, 1);
                m_saveWorker->setQImgFromIO(m_qImgFromIO);
                break;
            case RAW:
                m_saveWorker = new SavingWorker(fileName, 2);
                m_saveWorker->setRawData(m_rawData);
                m_saveWorker->setRawDataSize(m_rawDataSize);
                break;
            default:
                break;
            }

            m_saveWorker->moveToThread(m_saveThread);
            connect(m_saveWorker, SIGNAL(savingDone()), this, SLOT(onSavingDone()));
            connect(m_saveWorker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
            connect(m_saveThread, SIGNAL(started()), m_saveWorker, SLOT(process()));
            connect(m_saveWorker, SIGNAL(finished()), m_saveThread, SLOT(quit()));
            connect(m_saveWorker, SIGNAL(finished()), m_saveWorker, SLOT(deleteLater()));
            connect(m_saveThread, SIGNAL(finished()), m_saveThread, SLOT(deleteLater()));

            m_saveThread->start();
            m_progressDialog->exec();
        }
    }
    else{
        displayErrorMsg();
    }
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
    if(m_aboutDialog != NULL) delete m_aboutDialog;

    m_aboutDialog = new AboutDialog;

	m_aboutDialog->show();
}

void MainWindow::on_actionAnleitung_triggered()
{
    if(m_instructionDialog != NULL) delete m_instructionDialog;

    m_instructionDialog = new InstructionDialog;

	m_instructionDialog->show();
}

void MainWindow::onSendInformation(QString p_information, QString type){
    if(type.contains("LZW")){
        ui->tab4_textEdit_1->setText(p_information);
        m_tab3Calculating = false;
    }else if(type.contains("HUF")){
        ui->tab4_textEdit_2->setText(p_information);
    }else if(type.contains("RLC")){
        ui->tab4_textEdit_3->setText(p_information);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    foreach(QUrl url, event->mimeData()->urls()){
        if(QFileInfo(url.toLocalFile()).isFile()){
            event->acceptProposedAction();
            return;
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event)
 {
     if(!m_loading && !m_tab2Calculating && !m_tab3Calculating){
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
     else{
         displayErrorMsg();
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
			displayPicture(ui->tab1_graphicsView_1, m_stillPicture,0);
	}
}


void MainWindow::cellSelected()
{
    cout << "selected" << endl;
    int nRow = ui->tableWidget->currentIndex().row();
    //QPixmap localCopy(m_stillPicture);
    QPixmap localCopy(m_grayedOutPicture);
    QPainter p(&localCopy);
    QColor color;
    color = QColor(127,127,127);
    p.setPen(color);
    //Gif gif = *(static_cast<Gif*>(m_picFromIO));
    int k = 0;
    //for (int i = 0; i < gif.getFrame(0)->getHeight(); ++i) {
        //for (int j = 0; j < gif.getFrame(0)->getWidth(); ++j) {
    for (int nHeight = 0; nHeight < m_picFromIO->getHeight(); ++nHeight) {
        for (int nWidth = 0; nWidth < m_picFromIO->getWidth(); ++nWidth) {
            if(m_encodingVisual.getHighlightingArray()[(k++)+1] == nRow){
                p.drawPoint(nWidth, nHeight);
                cout << "pos: " << nRow << "hier: " << nHeight << " " << nWidth << endl;
            }
        }
    }
    if(!m_scenes.contains(30)){
        m_scenes.insert(30,new QGraphicsScene(this));
    }
    else{
        m_scenes[30]->clear();
    }

    ui->tab2_graphicsView_1->setScene(m_scenes[30]);

    m_scenes[30]->addPixmap(localCopy);
    ui->tab2_graphicsView_1->repaint();

    //ui->tab2_graphicsView_1->fitInView(m_scenes[30]->sceneRect(), Qt::KeepAspectRatio);
}

/*
bool MainWindow::isRedOnPos(int x, int y) {
    QColor color(m_stillPicture.toImage().pixel(x,y));
    if(color.red()>160 && color.blue()<50 && color.green()<50) {
        return true;
    }
    return false;
}
*/

void MainWindow::initTab1()
{
	if(!m_tab1Prepared){
		// PATRICK CODE GOES HERE. FEEL FREE TO CHANGE THE ABOVE CODE IN THIS METHOD IF YOU NEED TO
		// THIS METHOD IS CALLED EVERY TIME THE CORRESPONDING TAB GETS FOCUS

        // this if should be removed when m_stillPicture is also applied for animated pictures! Only the else row should be there.
        if(m_animated){
            //changeAnimGView(ui->tab2_graphicsView_1);
            //scalePicture(ui->tab2_graphicsView_1, ui->tab2_graphicsView_1->scene(), m_picFromIO->getWidth());

            m_grayedOutPicture = m_animatedPicture[0][0];
        }
        else{
            m_grayedOutPicture = m_stillPicture;
        }

        QPainter p(&m_grayedOutPicture);
        QColor color;
        color = QColor(0,0,0,200);
        p.setPen(color);
        for (int nHeight = 0; nHeight < m_picFromIO->getHeight(); ++nHeight) {
            for (int nWidth = 0; nWidth < m_picFromIO->getWidth(); ++nWidth) {
                p.drawPoint(nWidth, nHeight);
            }
        }

        ui->tab2_pushButton_1->setEnabled(true);
        ui->tab2_pushButton_2->setEnabled(true);
        QTableWidget *table = ui->tableWidget;
        Gif gif;
        if(m_mode == GIF) {
            gif = *(static_cast<Gif*>(m_picFromIO));
            IO::generateRawData(gif, 0, 0);
        } else {
            gif.setHeight(m_qImgFromIO.height());
            gif.setWidth(m_qImgFromIO.width());
            gif.extendFrames();
            gif.getFrame(0)->setHeight(m_qImgFromIO.height());
            gif.getFrame(0)->setWidth(m_qImgFromIO.width());

            unsigned char* pix = new unsigned char[m_qImgFromIO.height()*m_qImgFromIO.width()*3];

            int k = 0;
            for (int i = 0; i < m_qImgFromIO.height(); ++i) {
                for (int j = 0; j < m_qImgFromIO.width(); ++j) {
                    QColor rgb = QColor(m_qImgFromIO.pixel(j,i));
                    pix[k] = (unsigned char)rgb.red();
                    pix[k+1] = (unsigned char)rgb.green();
                    pix[k+2] = (unsigned char)rgb.blue();
                    k+=3;
                }
            }

            gif.getFrame(0)->setPixel(pix, m_qImgFromIO.height()*m_qImgFromIO.width()*3);
            gif.getFrame(0)->setDataFlag(0);
            gif.getFrame(0)->setLctFlag(1);
            IO::generateRawData(gif, 0, 1);
        }


        m_encodingVisual.startEncode(gif, 0);
        int currentRowCount = m_encodingVisual.getTable().size();

        table->setRowCount(currentRowCount);

        for (int i = 0; i < table->rowCount(); ++i) {
            CodeWord cW = m_encodingVisual.getTable()[i];
            QTableWidgetItem *newItem = new QTableWidgetItem(QString(cW.getSequenze().c_str()));
            table->setItem(i,0,newItem);
        }

        m_tab1Prepared = true;
        //should be deleted
        /*for (int x=0; x<=50; x+=1)
              m_scene->addLine(x,0,x,50, QPen(Qt::black));

        for (int y=0; y<=50; y+=1)
              m_scene->addLine(0,y,50,y, QPen(Qt::black));
        */

	}

    if(!m_loading){
        displayPicture(ui->tab2_graphicsView_1,m_grayedOutPicture, 0);
    }
}

void MainWindow::initTab2()
{

	ui->tab3_textEdit_1->setReadOnly(true);
	ui->tab3_textEdit_2->setReadOnly(true);
	ui->tab3_textEdit_1->setAcceptDrops(false);
	ui->tab3_textEdit_2->setAcceptDrops(false);

	if(!m_loading){
		if(m_animated){
			changeAnimGView(ui->tab3_graphicsView_1);
			scalePicture(ui->tab3_graphicsView_1, ui->tab3_graphicsView_1->scene(), m_picFromIO->getWidth());
		}
		else{
			displayPicture(ui->tab3_graphicsView_1, m_stillPicture, 0);
		}

	}

	if(!m_tab2Prepared){
        m_tab2Calculating = true;
		ui->tab3_textEdit_1->clear();
		ui->tab3_textEdit_2->clear();

		//Tab: Global local table conversion

		Gif* gif = static_cast<Gif*>(m_picFromIO);

		if(m_currLang == "de")
			ui->statusBar->showMessage("Generiere Bild(er)...");
		if(m_currLang == "en")
			ui->statusBar->showMessage("Generating Picture(s)...");

		QThread* conversionThread = new QThread;
		TableConversionWorker *conversionWorker = new TableConversionWorker(gif);
		conversionWorker->moveToThread(conversionThread);
		connect(conversionWorker, SIGNAL(conversionDone(Gif*)), this, SLOT(onTableConversionDone(Gif*)));
		connect(conversionWorker, SIGNAL(statisticsOut(TableConversionWorker::ConversionStatistics*)), this, SLOT(onStatisticsOut(TableConversionWorker::ConversionStatistics*)));
		connect(conversionWorker, SIGNAL(modeOut(TableConversionWorker::Mode*)), this, SLOT(onConversionModeOut(TableConversionWorker::Mode*)));
		connect(conversionWorker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
		connect(conversionThread, SIGNAL(started()), conversionWorker, SLOT(process()));
		connect(conversionWorker, SIGNAL(finished()), conversionThread, SLOT(quit()));
		connect(conversionWorker, SIGNAL(finished()), conversionWorker, SLOT(deleteLater()));
		connect(conversionThread, SIGNAL(finished()), conversionThread, SLOT(deleteLater()));
		conversionThread->start();

		m_tab2Prepared = true;
	}
}

void MainWindow::initTab3()
{
    if(!m_tab3Prepared){
        m_tab3Calculating = true;
        if(m_currLang == "en"){
            ui->tab4_textEdit_1->setText("calculating");
            ui->tab4_textEdit_2->setText("calculating");
            ui->tab4_textEdit_3->setText("calculating");
        }else if(m_currLang == "de"){
            ui->tab4_textEdit_1->setText("Berechnen");
            ui->tab4_textEdit_2->setText("Berechnen");
            ui->tab4_textEdit_3->setText("Berechnen");
        }

		unsigned char *rawData;
		int sizeOfRawData;


		if(m_mode == Mode::RAW){
			rawData = m_rawData;
			sizeOfRawData = m_rawDataSize;

		}else if(m_mode == Mode::PICTURE){

			unsigned char* pix = new unsigned char[m_qImgFromIO.height()*m_qImgFromIO.width()*3];

			int k = 0;
			for (int i = 0; i < m_qImgFromIO.height(); ++i) {
				for (int j = 0; j < m_qImgFromIO.width(); ++j) {
					QColor rgb = QColor(m_qImgFromIO.pixel(j,i));
					pix[k] = (unsigned char)rgb.red();
					pix[k+1] = (unsigned char)rgb.green();
					pix[k+2] = (unsigned char)rgb.blue();
					k+=3;
				}
			}

			rawData = pix;
			sizeOfRawData = m_qImgFromIO.height()*m_qImgFromIO.width()*3;
		}else{
			rawData = m_ioFile->getGif()->getFrame(0)->getPixel();
			sizeOfRawData = m_ioFile->getGif()->getFrame(0)->getSizeOfPixel();
		}



        QThread *huffmanThread = new QThread;
        CompressorWorker *huffmanCompressorWorker = new CompressorWorker();
        huffmanCompressorWorker->setCompressorType(HUFFMAN);
        huffmanCompressorWorker->setCurrLang(m_currLang);
        huffmanCompressorWorker->setRawData(rawData);
        huffmanCompressorWorker->setSizeOfRawData(sizeOfRawData);
        huffmanCompressorWorker->moveToThread(huffmanThread);

        connect(huffmanCompressorWorker, SIGNAL(sendInformation(QString, QString)), this, SLOT(onSendInformation(QString, QString)));
        connect(huffmanCompressorWorker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(huffmanThread, SIGNAL(started()), huffmanCompressorWorker, SLOT(process()));
        connect(huffmanCompressorWorker, SIGNAL(finished()), huffmanThread, SLOT(quit()));
        connect(huffmanCompressorWorker, SIGNAL(finished()), huffmanCompressorWorker, SLOT(deleteLater()));
        connect(huffmanThread, SIGNAL(finished()), huffmanThread, SLOT(deleteLater()));
        huffmanThread->start( );


        QThread *runLengthThread = new QThread;
        CompressorWorker *runLengthCompressorWorker = new CompressorWorker();
        runLengthCompressorWorker->setCompressorType(RunlengthEncoding);
        runLengthCompressorWorker->setCurrLang(m_currLang);
        runLengthCompressorWorker->setRawData(rawData);
        runLengthCompressorWorker->setSizeOfRawData(sizeOfRawData);
        runLengthCompressorWorker->moveToThread(runLengthThread);

        connect(runLengthCompressorWorker, SIGNAL(sendInformation(QString, QString)), this, SLOT(onSendInformation(QString, QString)));
        connect(runLengthCompressorWorker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(runLengthThread, SIGNAL(started()), runLengthCompressorWorker, SLOT(process()));
        connect(runLengthCompressorWorker, SIGNAL(finished()), runLengthThread, SLOT(quit()));
        connect(runLengthCompressorWorker, SIGNAL(finished()), runLengthCompressorWorker, SLOT(deleteLater()));
        connect(runLengthThread, SIGNAL(finished()), runLengthThread, SLOT(deleteLater()));
        runLengthThread->start( );


        QThread *lzwThread = new QThread;
        CompressorWorker *lzwCompressorWorker = new CompressorWorker();
        lzwCompressorWorker->setCompressorType(lZW);
        lzwCompressorWorker->setCurrLang(m_currLang);
        lzwCompressorWorker->setRawData(rawData);
        lzwCompressorWorker->setSizeOfRawData(sizeOfRawData);
        lzwCompressorWorker->moveToThread(lzwThread);

        connect(lzwCompressorWorker, SIGNAL(sendInformation(QString, QString)), this, SLOT(onSendInformation(QString, QString)));
        connect(lzwCompressorWorker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(lzwThread, SIGNAL(started()), lzwCompressorWorker, SLOT(process()));
        connect(lzwCompressorWorker, SIGNAL(finished()), lzwThread, SLOT(quit()));
        connect(lzwCompressorWorker, SIGNAL(finished()), lzwCompressorWorker, SLOT(deleteLater()));
        connect(lzwThread, SIGNAL(finished()), lzwThread, SLOT(deleteLater()));
        lzwThread->start( );
        m_tab3Prepared = true;
	}
}

void MainWindow::onTableConversionDone(Gif* p_gif){

	m_comparisonGif = p_gif;

	QThread* secondAnimPrepThread;
	AnimationPrepWorker* secondAnimPrepWorker;

	if(p_gif->getSizeOfFrames() == 1 || (p_gif->getSizeOfFrames() > 1 && checkDelayTime(p_gif))){ //gif is static
		m_stillPicture2 = generatePixmapFromPicture(p_gif);

		displayPicture(ui->tab3_graphicsView_2, m_stillPicture2, 1);

		//ui->statusBar->clearMessage();

		if(m_currLang == "de")
			ui->statusBar->showMessage("Erstelle Statistik... (kann etwas dauern)");
		if(m_currLang == "en")
			ui->statusBar->showMessage("Creating  statistics... (this'll take some time)");

	} else{

		secondAnimPrepThread = new QThread;
		secondAnimPrepWorker = new AnimationPrepWorker(p_gif);
		secondAnimPrepWorker->moveToThread(secondAnimPrepThread);
		connect(secondAnimPrepWorker, SIGNAL(pixArrayReady(QPixmap**)), this, SLOT(onComparisonPixArrayReady(QPixmap**)));
		connect(secondAnimPrepWorker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
		connect(secondAnimPrepThread, SIGNAL(started()), secondAnimPrepWorker, SLOT(process()));
		connect(secondAnimPrepWorker, SIGNAL(finished()), secondAnimPrepThread, SLOT(quit()));
		connect(secondAnimPrepWorker, SIGNAL(finished()), secondAnimPrepWorker, SLOT(deleteLater()));
		connect(secondAnimPrepThread, SIGNAL(finished()), secondAnimPrepThread, SLOT(deleteLater()));

		if(m_currLang == "de")
			ui->statusBar->showMessage("Bereite Animation vor...");
		if(m_currLang == "en")
			ui->statusBar->showMessage("Preparing Animation...");

		secondAnimPrepThread->start();
	}

}

void MainWindow::onStatisticsOut(TableConversionWorker::ConversionStatistics* p_statistics){

	ui->tab3_textEdit_1->clear();
	ui->tab3_textEdit_2->clear();

	QString stats1 = "", stats2 = "";

	switch (p_statistics->mode) {
	case TableConversionWorker::Mode::Global_to_Local:

		if(m_currLang == "de"){
			//ui->tab3_label_2->setText("Globale Tabelle");
			//ui->tab3_label_1->setText("Lokale Tabelle");

			stats1.append(QString("Globale Farbtabelle (Original)\n\n"));
			stats1.append(QString("LZW Zeit: %1 ms\n").arg(p_statistics->orgLZWTime));
			stats1.append(QString("Dateigröße: %1 Byte\n").arg(p_statistics->orgSize));

			stats2.append(QString("Lokale Farbtabelle (Generiert)\n\n"));
			stats2.append(QString("LZW Zeit: %1 ms\n").arg(p_statistics->newLZWTime));
			stats2.append(QString("Dateigröße: %1 Byte\n").arg(p_statistics->newSize));

			stats2.append(QString("\n\nUmwandlung dauerte: %1 ms").arg(p_statistics->conversionTime));

		}
		if(m_currLang == "en"){
			//ui->tab3_label_2->setText("Global Table");
			//ui->tab3_label_1->setText("Local Table");

			stats1.append(QString("Global color table (original)\n\n"));
			stats1.append(QString("LZW Time: %1 ms\n").arg(p_statistics->orgLZWTime));
			stats1.append(QString("Filesize: %1 byte\n").arg(p_statistics->orgSize));

			stats2.append(QString("Local color table (generated)\n\n"));
			stats2.append(QString("LZW Time: %1 ms\n").arg(p_statistics->newLZWTime));
			stats2.append(QString("Filesize: %1 byte\n").arg(p_statistics->newSize));

			stats2.append(QString("\n\nConversion took: %1 ms").arg(p_statistics->conversionTime));
		}

		break;
	case TableConversionWorker::Mode::Local_to_Global:

		if(m_currLang == "de"){
			//ui->tab3_label_1->setText("Globale Tabelle");
			//ui->tab3_label_2->setText("Lokale Tabelle");

			stats1.append(QString("Lokale Farbtabelle (Original)\n\n"));
			stats1.append(QString("LZW Zeit: %1 ms\n").arg(p_statistics->orgLZWTime));
			stats1.append(QString("Dateigröße: %1 Byte\n").arg(p_statistics->orgSize));

			stats2.append(QString("Globale Farbtabelle (Generiert)\n\n"));
			stats2.append(QString("LZW Zeit: %1 ms\n").arg(p_statistics->newLZWTime));
			stats2.append(QString("Dateigröße: %1 Byte\n").arg(p_statistics->newSize));

			stats2.append(QString("\n\nUmwandlung dauerte: %1 ms").arg(p_statistics->conversionTime));
		}
		if(m_currLang == "en"){
			//ui->tab3_label_1->setText("Global Table");
			//ui->tab3_label_2->setText("Local Table");

			stats1.append(QString("Local color table (original)\n\n"));
			stats1.append(QString("LZW Time: %1 ms\n").arg(p_statistics->orgLZWTime));
			stats1.append(QString("Filesize: %1 byte\n").arg(p_statistics->orgSize));

			stats2.append(QString("Global color table (generated)\n\n"));
			stats2.append(QString("LZW Time: %1 ms\n").arg(p_statistics->newLZWTime));
			stats2.append(QString("Filesize: %1 byte\n").arg(p_statistics->newSize));

			stats2.append(QString("\n\nConversion took: %1 ms").arg(p_statistics->conversionTime));
		}	

		break;
	default:
		break;
	}

	ui->tab3_textEdit_1->setText(stats1);
	ui->tab3_textEdit_2->setText(stats2);

	if(p_statistics != NULL)
		delete p_statistics;


	if(m_currLang == "de")
		ui->statusBar->showMessage(QString("Statistik wurde erstellt."), 3000);
	if(m_currLang == "en")
		ui->statusBar->showMessage(QString("Statistics have been created."), 3000);
    m_tab2Calculating = false;
}

void MainWindow::onComparisonPixArrayReady(QPixmap **p_pixArray){

	m_animatedPicture2 = p_pixArray;

	m_delaytimes2 = generateDelayTimeArray(m_comparisonGif);

	m_animationThread2.initPicture(m_comparisonGif, ui->tab3_graphicsView_2, m_animatedPicture2, m_comparisonGif->getSizeOfFrames(), m_delaytimes2);
	scalePicture(ui->tab3_graphicsView_2, ui->tab3_graphicsView_2->scene(), m_comparisonGif->getWidth());
	m_animationThread2.startAnim();

	/*if(m_currLang == "de")
		ui->statusBar->showMessage(QString("Ladevorgang beendet - Zoom: %1 x").arg(QString::number(ui->tab3_graphicsView_1->transform().m11(),'f',2)), 3000);
	if(m_currLang == "en")
		ui->statusBar->showMessage(QString("Loading done - Zoom: %1 x").arg(QString::number(ui->tab3_graphicsView_1->transform().m11(),'f',2)), 3000);*/


	if(m_currLang == "de")
		ui->statusBar->showMessage("Erstelle Statistik... (kann etwas dauern)");
	if(m_currLang == "en")
		ui->statusBar->showMessage("Creating  statistics... (this'll take some time)");
}

void MainWindow::onConversionModeOut(TableConversionWorker::Mode* p_mode){

	switch (*p_mode) {
	case TableConversionWorker::Mode::Global_to_Local:

		if(m_currLang == "de"){
			ui->tab3_label_2->setText("Globale Tabelle");
			ui->tab3_label_1->setText("Lokale Tabelle");
		}
		if(m_currLang == "en"){
			ui->tab3_label_2->setText("Global Table");
			ui->tab3_label_1->setText("Local Table");
		}

		break;
	case TableConversionWorker::Mode::Local_to_Global:

		if(m_currLang == "de"){
			ui->tab3_label_1->setText("Globale Tabelle");
			ui->tab3_label_2->setText("Lokale Tabelle");
		}
		if(m_currLang == "en"){
			ui->tab3_label_1->setText("Global Table");
			ui->tab3_label_2->setText("Local Table");
		}
	default:
		break;
	}

}

void MainWindow::on_tab2_pushButton_1_released()
{
    size_t lastSize = m_encodingVisual.getTable().size();
    while(m_encodingVisual.getI() < m_encodingVisual.getSizeOfRawData() && m_encodingVisual.getTable().size() == lastSize){
        updateGreyOutPicture();
        m_encodingVisual.nextStep();
        if(m_encodingVisual.getTable().size() != lastSize){
             addNewRowContent();
        }
    }

    if(m_encodingVisual.getI() >= m_encodingVisual.getSizeOfRawData()) {
        ui->tab2_pushButton_1->setDisabled(true);
        ui->tab2_pushButton_2->setDisabled(true);
    }
}

void MainWindow::on_tab2_pushButton_2_released()
{
    if(m_picFromIO->getHeight()*m_picFromIO->getWidth()>6666) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(highlightingMessageBoxText(true));
        msgBox.setText(highlightingMessageBoxText(false));
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);

        if(m_currLang=="de") {
            msgBox.setButtonText(QMessageBox::Yes, "Ja");
            msgBox.setButtonText(QMessageBox::No, "Nein");
        }
        if(msgBox.exec() == QMessageBox::No) {
            return;
        }
    }
    //m_encodingVisual.resetInternalState();

    size_t lastSize = m_encodingVisual.getTable().size();
    while(m_encodingVisual.getI() < m_encodingVisual.getSizeOfRawData()) {
        updateGreyOutPicture();
        m_encodingVisual.nextStep();
        if(m_encodingVisual.getTable().size() != lastSize) {
            addNewRowContent();
        }
        lastSize = m_encodingVisual.getTable().size();
    }
    ui->tab2_pushButton_1->setDisabled(true);
    ui->tab2_pushButton_2->setDisabled(true);  
}

QString MainWindow::highlightingMessageBoxText(bool title) {
    if(title) {
        if(m_currLang == "de"){
            return "Achtung";
        }
        if(m_currLang == "en"){
            return "Attention";
        }
    } else {
        if(m_currLang == "de"){
            return QString("Der folgende Befehl wird eine sehr lange Rechenzeit in Anspruch nehmen.\nDer Algorithmus muss %1 Pixel durchlaufen.\n\nWollen Sie trotzdem fortfahren?").arg(m_picFromIO->getHeight()*m_picFromIO->getWidth());
        }
        if(m_currLang == "en"){
            return QString("The following command will need a long calculating time.\nThe algorithm have to run through %1 pixels\n\nDo you still wish to continue?").arg(m_picFromIO->getHeight()*m_picFromIO->getWidth());
        }
    }
    return "";
}

void MainWindow::addNewRowContent() {
    QTableWidget *qTable = ui->tableWidget;
    qTable->setRowCount(qTable->rowCount()+1);

    CodeWord cW = m_encodingVisual.getTable()[m_encodingVisual.getTable().size()-1];
    QTableWidgetItem *newItem = new QTableWidgetItem(QString(cW.getSequenze().c_str()));
    qTable->setItem(m_encodingVisual.getTable().size()-1,0,newItem);
    qTable->QAbstractItemView::scrollToBottom();
}

void MainWindow::updateGreyOutPicture() {
    QPainter p(&m_grayedOutPicture);
    int x = m_encodingVisual.getI()%m_picFromIO->getWidth();
    int y = m_encodingVisual.getI()/m_picFromIO->getWidth();
    QColor color(m_stillPicture.toImage().pixel(x,y));
    p.setPen(color);
    p.drawPoint(x,y);

    if(!m_scenes.contains(30)){
        m_scenes.insert(30,new QGraphicsScene(this));
    }
    else{
        m_scenes[30]->clear();
    }

    ui->tab2_graphicsView_1->setScene(m_scenes[30]);
    m_scenes[30]->addPixmap(m_grayedOutPicture);
    ui->tab2_graphicsView_1->repaint();
}
