#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "animationThread.h"
#include "picture.h"
#include "io.h"
#include "aboutdialog.h"
#include "instructiondialog.h"
#include <QMainWindow>
#include <QPixmap>
#include <QGraphicsView>
#include <QTranslator>
#include <QTimer>
#include <QTextEdit>

namespace Ui {
class mainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

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
    void repaint(); //Test. Kein repaint aus Thread -> hier slot mit repaint funktion in Thread signal das per emit ausgelöst wird
    void repaint2(); //Test
    void repaint3(); //Test

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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

    void on_actionGIF_Bild_triggered();

    void on_action_ber_triggered();

    void on_actionAnleitung_triggered();

    void on_actionLokale_Globale_Tabellen_Vergleichsbild_triggered();

private:
    Ui::mainWindow *ui; // ui Pointer for the GUI elements (Button), left as is -> generated by QT
    AboutDialog *m_aboutDialog; // Dialog for Menu 'Help->about...'
    InstructionDialog *m_instructionDialog; // Dialog for Menu 'Help->instruction'
    QPixmap *pixArray /*Test*/, m_drawPicture; // Pixmap that is drawn on the GUI (this pixmap -> QGraphicsScene -> QGraphicsView -> displayed in GUI)
    AnimationThread aT1, aT2, aT3; //Test
    QGraphicsScene *p_scene, *scene2, *scene3; //Test
    QTranslator m_translator; // contains the translations for this application
    QString m_currLang;     // contains the currently loaded language
    QString m_langPath;     // Path of language files. This is always fixed to /languages.
    QString m_headerInfo; // Contains Headerinfo
    IO m_ioFile;    // File of Pic that gets preloaded
    Picture *m_picFromIO; // Picture generated from m_ioFile
    int m_tabPosition; // Currently shown Tab

    void drawPicture();  //Test
    void drawAnimatedPicture(); //Test
    QPixmap generatePixmap(/*int *colorTable,*/ int width, int height); //Test
    /**
     * @brief loads and displays the First Picture into the GUI
     *
     */
    void guiSetup();
    /**
     * @brief
     * loads a language by the given language shortcur (e.g. de, en, ...)
    */
    void loadLanguage(const QString& rLanguage);
    /**
     * @brief creates the language menu dynamically from the content of m_langPath
    */
    void createLanguageMenu(void);
    /**
     * @brief generates a Pixmap from pic that is mainly used to be drawn on the GUI
     *
     * @param pic - The loaded Picture
     * @return QPixmap Pixmap that gets drawn onto the GUI
     */
    QPixmap generatePixmapFromPicture(Picture *p_pic);
    /**
     * @brief display p_pic in p_view on the GUI
     *
     * @param p_view QGraphicsView in GUI that should display p_pic
     * @param p_pic pixmap that will be shown an GUI
     */
    void displayPicture(QGraphicsView *p_view, QPixmap &p_pic);
    /**
     * @brief Displays Header Information from p_picFromIO onto p_textEdit
     *
     * @param p_textEdit Textfield that displays the Headerinformation
     * @param p_picFromIO Picture that contains the Headerinformation
     */
    void displayHeaderInfo(QTextEdit *p_textEdit, Picture *p_picFromIO);
    /**
     * @brief Auto scales The picture inside p_scene on p_view in relation to p_pictureWidth
     *
     * @param p_view View that contains p_scene
     * @param p_scene Scene that contains picture to scale
     * @param p_pictureWidth Width of Picture to scale
     */
    void scalePicture(QGraphicsView *p_view, QGraphicsScene *p_scene, int p_pictureWidth);
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
     * @brief Triggerd by pushing the "Exit" button on the main Window
     *
     * @param event Standard QT Close Event
     */
    void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
