#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "animationThread.h"
#include "picture.h"
#include "io.h"
#include "gif.h"
#include "aboutdialog.h"
#include "instructiondialog.h"
#include "tableconversionworker.h"
#include <loadingworker.h>
#include <animationprepworker.h>
#include <QMainWindow>
#include <QPixmap>
#include <QGraphicsView>
#include <QTranslator>
#include <QTimer>
#include <QTextEdit>
#include <QProgressBar>
#include <fstream>
#include <iostream>


namespace Ui {
class mainWindow;
}
/**
 * @brief Main GUI Window
 *
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::mainWindow *ui; // ui Pointer for the GUI elements (Button), left as is -> generated by QT
    AboutDialog *m_aboutDialog; // Dialog for Menu 'Help->about...'
    InstructionDialog *m_instructionDialog; // Dialog for Menu 'Help->instruction'
    QTranslator m_translator; // contains the translations for this application
    QGraphicsScene *m_scene; //Scene Pointer to display a Picture on a View
    QString m_currLang;     // contains the currently loaded language
    QString m_langPath;     // Path of language files. This is always fixed to /languages.
    QString m_headerInfo; // Contains Headerinfo
    QPixmap **m_animatedPicture; //Array of Frames for an animated GIF
    QPixmap m_stillPicture; // Pixmap that is drawn on the GUI (this pixmap -> QGraphicsScene -> QGraphicsView -> displayed in GUI)
    Picture *m_picFromIO; // Picture generated from m_ioFile
    QImage m_qImgFromIO; // Picture (everything beside GIF) loaded via QT Classes
    AnimationThread m_animationThread; //"Thread" that display the animated Picture
    QThread *m_loadThread;  // Thread to load pictures with
    QThread *m_animPrepThread; // Thread to prepare the Animation
    LoadingWorker *m_loadWorker;  // Worker for m_loadThread
    AnimationPrepWorker *m_animPrepWorker; //Worker for m_animPrepThread
    IO *m_ioFile; //File from IO
    int *m_delaytimes; //Array that contains delaytimes of an animated GIF
    bool m_animated; // True if current Picture is animated
    bool m_loading; // True if Programm is currently loading
    bool m_lastFileWasRaw; //True if the file that was loaded previously was a Raw Data File
    bool m_tab1Prepared; // False if the Calculations for tab1 aren't done
    bool m_tab2Prepared; // False if the Calculations for tab2 aren't done
    bool m_tab3Prepared; // False if the Calculations for tab3 aren't done
    enum Mode {GIF, PICTURE, RAW} m_mode; //Currently Active Mode
    unsigned char *m_rawData;  // Raw Data of the loaded file
    int m_rawDataSize; //Size of m_rawData

    /**
     * @brief Loads and displays the First Picture into the GUI
     *
     */
    void guiSetup();
    /**
     * @brief Sets drag&drop behaviour
     *
     */
    void dropSetup();
    /**
     * @brief
     * Loads a language by the given language shortcur (e.g. de, en, ...)
    */
    void loadLanguage(const QString& rLanguage);
    /**
     * @brief Creates the language menu dynamically from the content of m_langPath
    */
    void createLanguageMenu(void);
    /**
     * @brief Generates a Pixmap from a member of the class Picture that is mainly used to be drawn on the GUI
     *
     * @param pic - The loaded Picture
     * @return QPixmap Pixmap that gets drawn onto the GUI
     */
    QPixmap generatePixmapFromPicture(Picture *p_pic);
    /**
     * @brief Loads a picture from p_filePath.
     *
     * @param p_filePath Filepath to load Picture from
     * @return bool True if load was successful, false otherwise
     */
    bool loadFile(QString p_filePath);
    /**
     * @brief Checks gif to see if the frame delaytimes differ from 0
     *
     * @return bool True if a single delayTime is != 0
     */
    bool checkDelayTime(Gif *p_gif);
    /**
     * @brief Generates an int Array that contains the Frame Delaytimes
     *
     * @param gif GIF that provides the delaytimes
     * @return int Array filled with delaytimes from gif
     */
    int *generateDelayTimeArray(Gif *p_gif);
    /**
     * @brief Display p_pic in p_view on the GUI
     *
     * @param p_view QGraphicsView in GUI that should display p_pic
     * @param p_pic pixmap that will be shown an GUI
     */
    void displayPicture(QGraphicsView *p_view, QPixmap &p_pic);
    /**
     * @brief Displays Header Information from p_picFromIO onto p_textEdit
     *
     * @param p_textEdit p_textEdit Textfield that displays the Headerinformation
     * @param p_textEdit2 p_textEdit Textfield that displays the Frameinformation
     * @param p_picFromIO Picture that contains the Headerinformation
     */
    void displayHeaderInfo(QTextEdit *p_textEdit, QTextEdit *p_textEdit2, Picture *p_picFromIO);
    /**
     * @brief Displays Header Information from p_qImgFromIO onto p_textEdit
     *
     * @param p_textEdit Textfield that displays the Headerinformation
     * @param p_qImgFromIO QImage that contains the Headerinformation
     */
    void displayHeaderInfo(QTextEdit *p_textEdit, QImage &p_qImgFromIO);
    /**
     * @brief Automatically scales the picture inside p_scene on p_view in relation to p_pictureWidth
     *
     * @param p_view View that contains p_scene
     * @param p_scene Scene that contains picture to scale
     * @param p_pictureWidth Width of Picture to scale
     */
    void scalePicture(QGraphicsView *p_view, QGraphicsScene *p_scene, int p_pictureWidth);
    /**
     * @brief Setup of Tab0. Gets called when Tab0 gets focus. Displays the picture
     *
     */
    void initTab0();
    /**
     * @brief Setup of Tab1. Gets called when Tab1 gets focus. Displays the picture and does other necessary preparations
     *
     */
    void initTab1();
    /**
     * @brief Setup of Tab2. Gets called when Tab2 gets focus. Displays the picture and does other necessary preparations
     *
     */
    void initTab2();
    /**
     * @brief Setup of Tab3. Gets called when Tab3 gets focus. Does necessary preparations
     *
     */
    void initTab3();
    /**
     * @brief changes the display of the animated Picture to p_gView
     *
     * @param p_gView View to show the animated Picture on
     */
    void changeAnimGView(QGraphicsView *p_gView);
    /**
     * @brief Checks for various Shortcuts to make using the GUI easier
     *
     * @param event Standard QT Key Event
     */
    void keyPressEvent(QKeyEvent *event);
    /**
     * @brief Zoom for Graphicsview on first Tab
     *
     * @param event Standard QT MouseWheel Event
     */
    void wheelEvent(QWheelEvent *event);
    /**
     * @brief Called by pushing the "Exit" button on the main Window
     *
     * @param event Standard QT Close Event
     */
    void closeEvent(QCloseEvent *event);
    /**
     * @brief Called by draging a File on the Main Window
     *
     * @param event Standard QT DragEnter Event
     */
    void dragEnterEvent(QDragEnterEvent *event);
    /**
     * @brief Called by droping a File into Main Window
     *
     * @param event Standard QT Drop Event
     */
    void dropEvent(QDropEvent *event);


private slots:
    /**
     * @brief Called on Keypush 'ESC' or via Menu 'File->Quit'
     *
     */
    void on_actionBeenden_triggered();
    /**
     * @brief Opens 'File open' Dialog that loads a new Picture and changes focus to Tab1
     *
     */
    void on_actionDatei_ffnen_triggered();
    /**
     * @brief Opens 'File save' Dialog that saves the current Picture as a GIF
     *
     */
    void on_actionGIF_Bild_triggered();
    /**
     * @brief Opens Shows Aboutdialog
     *
     */
    void on_action_ber_triggered();
    /**
     * @brief Show Instructiondialog
     *
     */
    void on_actionAnleitung_triggered();
    /**
     * @brief 'File save' Dialog that saves the current Picture as a GIF concerning the Local/Global Tables
     *
     */
    void on_actionLokale_Globale_Tabellen_Vergleichsbild_triggered();
    /**
     * @brief Trigges tab[X]Setup() method [x = index] to prepare Picture Display and other Calculations.
     *
     * @param index Tab that got selected
     */
    void on_tabWidget_currentChanged(int index);

protected:
    /**
     * @brief this event is called, when a new translator is loaded or the system language is changed
    */
    void changeEvent(QEvent*);


protected slots:
    /**
     * @brief this slot is called by the language menu actions
    */
    void slotLanguageChanged(QAction* action);
    /**
     * @brief slot for Error usage
     *
     * @param error Error message
     */
    void errorString(QString error);
    /**
     * @brief slot to repaint p_gView
     *
     * @param p_gView GraphicsView that gets repainted
     */
    void repaint(QGraphicsView *p_gView);
    /**
     * @brief m_loadWorker calls this slots once it is done loading the picture
     *
     * @param p_pic Picture that is loaded via m_loadWorker
     */
    void onPicReady(Picture *p_pic);
    void onIOReady(IO *p_io);
    /**
     * @brief m_animPrepWorker calls this slots once it is done preparing the Animation Pictures
     *
     * @param p_pixArray Pixmap Array that is animated on screen
     */
    void onPixArrayReady(QPixmap **p_pixArray);
	/**
	 * @brief TableConversionWorker calls this slot when the conversion is done
	 *
	 * @param p_gif The gif that was created in the conversion
	 */
	void onTableConversionDone(Gif* p_gif);
	/**
	 * @brief TableConversionWorker calls this slot when the conversion statistics are ready to display
	 *
	 * @param p_statistics The statistics of the conversion, wrapped in a struct
	 */
	void onStatisticsOut(TableConversionWorker::ConversionStatistics* p_statistics);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
};
#endif // MAINWINDOW_H
