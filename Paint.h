#ifndef PAINT_H
#define PAINT_H

#include <paintWidget.h>
#include <QList>
#include <QSettings>
#include <QMainWindow>
#include <QLabel>


// ScribbleArea used to paint the image
class ScribbleArea;

class MainWindow : public QMainWindow
{
    // Declares our class as a QObject which is the base class
    // for all Qt objects
    // QObjects handle events
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void language(QString language);

protected:
    // Function used to close an event
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

// The events that can be triggered
private slots:
    void open();
    void save();
    void saveAs();
    void backgroundColor();
    void penColor();
    void penWidth();
    void choosePen(paintWidget::drawMode mode);
    void about();
    void retranslate();
    void loadSettings();
    void saveSettings();
    void restoreDefault();
    void updateStatusbar();

private:
    // settings
    QSettings *settings;
    QString path = "";
    QString currLang;
    QString imageFormat = "";

    void clear();

    // Will tie user actions to functions
    void createActions();
    void createMenus();
    void createToolbar();
    void createStatusBar();

    // Will check if changes have occurred since last save
    bool maybeSave();

    // Opens the Save dialog and saves
    bool saveFile(const QByteArray &fileFormat);

    // What we'll draw on
    paintWidget *scribbleArea;

    // The menu widgets
    QMenu *saveAsMenu;
    QMenu *fileMenu;
    QMenu *optionMenu;
    QMenu *langMenu;
    QMenu *helpMenu;

    // All the actions that can occur
    QAction *openAct;

    // Actions tied to specific file formats
    QAction *saveAct;
    QList<QAction *> saveAsActs;
    QAction *previewAct;
    QAction *exitAct;
    QAction *backColAct;
    QAction *penColorAct;
    QAction *penWidthAct;
    QAction *penAct;
    QAction *printAct;
    QAction *clearScreenAct;
    QAction *germanAct;
    QAction *englishAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    // Toolbar
    QToolBar *toolbar;

    // STatusbar
    QLabel *sizeLabel;
};

#endif
