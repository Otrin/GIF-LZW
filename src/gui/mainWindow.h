#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "animationThread.h"
#include <QMainWindow>
#include <QPixmap>
#include <QTranslator>
#include <QTimer>
namespace Ui {
class mainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    /** this event is called, when a new translator is loaded or the system language is changed
     */
    void changeEvent(QEvent*);

protected slots:
    /** this slot is called by the language menu actions
      */
    void slotLanguageChanged(QAction* action);
    void errorString(QString error);
    void repaint(); //Kein repaint aus Thread -> hier slot mit repaint funktion in Thread signal das per emit ausgelöst wird
    void repaint2();
    void repaint3();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::mainWindow *ui;
    QPixmap *pixArray;
    AnimationThread aT1, aT2, aT3;
    QGraphicsScene *scene, *scene2, *scene3;
    QTranslator m_translator;   /**< contains the translations for this application */
    QString m_currLang;     /**< contains the currently loaded language */
    QString m_langPath;     /**< Path of language files. This is always fixed to /languages. */
    void drawPicture();
    void drawAnimatedPicture();
    void guiSetup();
    QPixmap generatePixmap(/*int *colorTable,*/ int width, int height);
    /** loads a language by the given language shortcur (e.g. de, en, ...) */
    void loadLanguage(const QString& rLanguage);
    /** creates the language menu dynamically from the content of m_langPath */
    void createLanguageMenu(void);
};
#endif // MAINWINDOW_H
