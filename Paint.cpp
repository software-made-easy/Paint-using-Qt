#include <QtWidgets>
#include <QTranslator>
#include <QLibraryInfo>
#include <QSettings>

#ifdef Q_OS_ANDROID
#include <QStandardPaths>
#include <QtColorWidgets/color_dialog.hpp>
#endif

#include "Paint.h"
#include "paintWidget.h"


#if defined(Q_OS_BLACKBERRY) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(Q_OS_WP)
#define Q_OS_MOBILE
#else
#define Q_OS_DESKTOP
#endif


QTranslator translator, qtTranslator;
QString translationsPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);


// MainWindow constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // set window icon
    setWindowIcon(QIcon(":/assets/Paint.png"));

    // Create the ScribbleArea widget and make it
    // the cenQApplication::tral widget
    scribbleArea = new paintWidget(this);
    setCentralWidget(scribbleArea);

    // Create actions and menus
    createActions();
    createMenus();
    createToolbar();
#if defined(QT_DEBUG) && !defined(Q_OS_MOBILE)
    createStatusBar();

    // update status bar
    connect(scribbleArea, &paintWidget::updateStatusBar, this, &MainWindow::updateStatusbar);
#endif

    // Set the title
    qApp->setApplicationDisplayName(QApplication::translate("MainWindow", "Paint"));

    // create settings manager
#ifdef Q_OS_ANDROID
    settings = new QSettings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/SME_PAINT.ini", QSettings::IniFormat);
#else
    settings = new QSettings("SME", "Paint", this);
#endif

    loadSettings();
}

// User QApplication::tried to close the app
void MainWindow::closeEvent(QCloseEvent *event)
{
    // If they QApplication::try to close maybeSave() returns QApplication::true
    // if no changes have been made and the app closes
    if (maybeSave()) {
        saveSettings();
        event->accept();
    } else {

        // If there have been changes ignore the event
        event->ignore();
    }
}

void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    else {
        QWidget::changeEvent(event);
    }
}

void MainWindow::retranslate() {
    fileMenu->setTitle(QApplication::translate("MainWindow", "&File"));
    openAct->setText(QApplication::translate("MainWindow", "&Open"));
    saveAct->setText(QApplication::translate("MainWindow", "Save"));
    saveAsMenu->setTitle(QApplication::translate("MainWindow", "&Save As"));
    printAct->setText(QApplication::translate("MainWindow", "&Print"));
    previewAct->setText(QApplication::translate("MainWindow", "Print Preview"));
    exitAct->setText(QApplication::translate("MainWindow", "E&xit"));
    optionMenu->setTitle(QApplication::translate("MainWindow", "&Options"));
    backColAct->setText(QApplication::tr("Change background color"));
    penColorAct->setText(QApplication::translate("MainWindow", "&Pen Color"));
    penWidthAct->setText(QApplication::translate("MainWindow", "Pen &Width"));
    penAct->setText(QApplication::translate("MainWindow", "Use Rubber"));
    clearScreenAct->setText(QApplication::translate("MainWindow", "&Clear Screen"));
    langMenu->setTitle(QApplication::translate("MainWindow", "Language"));
    germanAct->setText(QApplication::translate("MainWindow", "German"));
    englishAct->setText(QApplication::translate("MainWindow", "English"));
    helpMenu->setTitle(QApplication::translate("MainWindow", "&Help"));
    aboutAct->setText(QApplication::translate("MainWindow", "&About"));
    aboutQtAct->setText(QApplication::translate("MainWindow", "About &Qt"));
    qApp->setApplicationDisplayName(QApplication::translate("MainWindow", "Paint"));
}

void MainWindow::language(QString language) {
    if (language != "en") {
        if (translator.load(QString(":/i18n/Paint_%1").arg(language))) {
            qApp->installTranslator(&translator);
        }
        if (qtTranslator.load(":/assets/qtbase_" + language)) {
            qApp->installTranslator(&qtTranslator);
        }
        currLang = language;
    }
    else {
        qApp->removeTranslator(&translator);
        qApp->removeTranslator(&qtTranslator);
    }
}

// Settings management
void MainWindow::loadSettings() {
    resize(settings->value("size", QSize(500, 500)).toSize());

    QPoint position = settings->value("pos", QPoint(710, 270)).toPoint();
    if (!position.isNull()) {
        move(position);
    }
    else {
        move(QPoint(710, 270));
    }

    currLang = settings->value("lang", QString()).toString();
    if (currLang.isEmpty()) {
        currLang = QLocale::system().name().split("_")[0];
    }

    if (currLang != "en") {
        language(currLang);
    }
    restoreState(settings->value("state", QByteArray()).toByteArray());
}

void MainWindow::saveSettings() {
    settings->setValue("size", size());
    settings->setValue("pos", pos());
    settings->setValue("lang", currLang);
    settings->setValue("state", saveState());
    settings->sync();
}

void MainWindow::restoreDefault() {
    settings->setValue("size", QSize(500, 500));
    settings->setValue("pos", QPoint(710, 270));
}

// Check if the current image has been changed and then
// open a dialog to open a file
void MainWindow::open()
{
    // Check if changes have been made since last save
    // maybeSave() returns QApplication::true if no changes have been made
    if (maybeSave()) {
        QString types;
        foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
            types = types + " *." + QString(format);
        }
        QString format = QApplication::tr("Images") + "(" + types + ")";
        // Get the file to open from a dialog
        // QApplication::tr sets the window title to Open File
        // QDir opens the current dirctory
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        QApplication::translate("MainWindow", "Open File"),
                                                        QDir::currentPath(), format
                                                        );

        // If we have a file name load the image and place
        // it in the scribbleArea
        if (!fileName.isEmpty()) {
            path = fileName;
            imageFormat = QFileInfo(fileName).suffix();
            scribbleArea->openImage(fileName);
        }
    }
}

void MainWindow::save() {
    if (imageFormat.isEmpty()) {
        QStringList types;
        foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
            types.append(QString(format));
        }
        bool ok;

        imageFormat = QInputDialog::getItem(this, QApplication::tr("Select format"), QApplication::tr("Select image format"), types,
                                            types.indexOf("png"), true, &ok);
        if (!ok) {
            return;
        }
    }

    if (path.isEmpty()) {
        saveFile(imageFormat.toLocal8Bit());
    }
    else {
        scribbleArea->saveImage(path, imageFormat.toLocal8Bit().data());
    }
}

// Called when the user clicks Save As in the menu
void MainWindow::saveAs()
{
    // A QAction represents the action of the user clicking
    QAction *action = qobject_cast<QAction *>(sender());

    // Stores the array of bytes of the users data
    QByteArray fileFormat = action->data().toByteArray();

    // Pass it to be saved
    saveFile(fileFormat);
}

// Opens a dialog to change background(color)
void MainWindow::backgroundColor() {
    // Store the chosen color from the dialog
    QColor newColor;

#ifdef Q_OS_ANDROID
    color_widgets::ColorDialog dialog(this);
    dialog.setColor(scribbleArea->penColor());
    dialog.setButtonMode(color_widgets::ColorDialog::OkCancel);
    connect(&dialog, QOverload<QColor>::of(&color_widgets::ColorDialog::colorSelected), this,
            [&newColor](QColor color){newColor = color;});
    dialog.exec();
#else
    newColor = QColorDialog::getColor(scribbleArea->backgroundColor, this);
#endif

    // If color is valid, set it
    if (newColor.isValid())
        scribbleArea->setBackgroundColor(newColor);
}

// Opens a dialog to change the pen color
void MainWindow::penColor()
{
    // Store the chosen color from the dialog
    QColor newColor;

#ifdef Q_OS_ANDROID
    color_widgets::ColorDialog dialog(this);
    dialog.setColor(scribbleArea->penColor());
    dialog.setButtonMode(color_widgets::ColorDialog::OkCancel);
    connect(&dialog, QOverload<QColor>::of(&color_widgets::ColorDialog::colorSelected), this,
            [&newColor](QColor color){newColor = color;});
    dialog.exec();
#else
    newColor = QColorDialog::getColor(scribbleArea->penColor(), this);
#endif

    // If color is valid, set it
    if (newColor.isValid())
        scribbleArea->setPenColor(newColor);
}

// Opens a dialog that allows the user to change the pen width
void MainWindow::penWidth()
{
    // Stores button value
    bool ok;

    // QApplication::translate("MainWindow", "Paint") is the title
    // the next QApplication::tr is the text to display
    // Get the current pen width
    // Define the min, max, step and ok button
    int newWidth = QInputDialog::getInt(this, QApplication::translate("MainWindow", "Paint"),
                                        QApplication::translate("MainWindow", "Select pen width:"),
                                        scribbleArea->penWidth(),
                                        1, 2000, 1, &ok);
    // Change the pen width
    if (ok) {
        scribbleArea->setPenWidth(newWidth);
    }
}

// Pen mode
void MainWindow::choosePen(paintWidget::drawMode mode) {
    scribbleArea->setPenMode(mode);

    if (mode == paintWidget::Auto) {
        mode = scribbleArea->mode;
    }

    if (mode == paintWidget::Normal) {
        penAct->setChecked(false);
    }
    else {
        penAct->setChecked(true);
    }
}

// Open an about dialog
void MainWindow::about()
{
    // Window title and text to display
    QMessageBox::about(this, QApplication::translate("MainWindow", "About Paint"),
            QApplication::translate("MainWindow", "<p><b>Paint</b> is a simple tool for draw images.</p>"));
}

// Define menu actions that call functions
void MainWindow::createActions()
{
    // Create the action tied to the menu
    openAct = new QAction(QIcon::fromTheme("document-open"), QApplication::translate("MainWindow", "&Open"), this);

    // Define the associated shortcut key
    openAct->setShortcuts(QKeySequence::Open);

    // Tie the action to MainWindow::open()
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    // create save action
    saveAct = new QAction(QIcon::fromTheme("document-save"), QApplication::translate("MainWindow", "Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);

    // Get a list of the supported file formats
    // QImageWriter is used to write images to files
    foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
        // Create an action for each file format
        QAction *action = new QAction(QString(format), this);

        // Set an action for each file format
        action->setData(format);

        // When clicked call MainWindow::saveAs()
        connect(action, SIGNAL(triggered()), this, SLOT(saveAs()));

        // Attach each file format option menu item to Save As
        saveAsActs.append(action);
    }

    // Create print action and tie to MainWindow::print()
    printAct = new QAction(QIcon::fromTheme("document-print"), QApplication::translate("MainWindow", "&Print"), this);
    printAct->setShortcuts(QKeySequence::Print);
    connect(printAct, SIGNAL(triggered()), scribbleArea, SLOT(print()));

    // create print preview action
    previewAct = new QAction(QIcon::fromTheme("document-print-preview"), QApplication::translate("MainWindow", "Print Preview"), this);
    previewAct->setShortcut(QKeySequence("Ctrl+Shift+P"));
    connect(previewAct, SIGNAL(triggered()), scribbleArea, SLOT(preview()));

    // Create exit action and tie to MainWindow::close()
    exitAct = new QAction(QIcon::fromTheme("application-exit"), QApplication::translate("MainWindow", "E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    // Create pen color action and tie to MainWindow::penColor()
    penColorAct = new QAction(QIcon::fromTheme("color-management"), QApplication::translate("MainWindow", "&Pen Color"), this);
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));

    // Create pen width action and tie to MainWindow::penWidth()
    penWidthAct = new QAction(QIcon::fromTheme(":/assets/images/pen-width.png"), QApplication::translate("MainWindow", "Pen &Width"), this);
    connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));

    // create action to choose between pen and rubber
    penAct = new QAction(QIcon::fromTheme(":/assets/images/eraser.png"), QApplication::tr("Use Rubber"), this);
    penAct->setCheckable(true);
    connect(penAct, &QAction::triggered, this, [this](){choosePen(paintWidget::Auto); });

    // create choose background color action
    backColAct = new QAction(QIcon(":/assets/images/background-color.png"), QApplication::tr("Change background color"), this);
    connect(backColAct, &QAction::triggered, this, &MainWindow::backgroundColor);

    // Create clear screen action and tie to MainWindow::clearImage()
    clearScreenAct = new QAction(QIcon::fromTheme("edit-clear"), QApplication::translate("MainWindow", "&Clear Screen"), this);
    clearScreenAct->setShortcut(QApplication::translate("MainWindow", "Ctrl+L"));
    connect(clearScreenAct, &QAction::triggered, this, &MainWindow::clear);

    // Create language actions
    germanAct = new QAction(QIcon(":/assets/flags/german.svg"), QApplication::translate("MainWindow", "German"), this);
    connect(germanAct, &QAction::triggered, this, [this]{ language("de"); });

    englishAct = new QAction(QIcon(":/assets/flags/united_kingdom.svg"),QApplication::translate("MainWindow", "English"), this);
    connect(englishAct, &QAction::triggered, this, [this]{ language("en"); });

    // Create about action and tie to MainWindow::about()
    aboutAct = new QAction(QIcon(":/assets/Paint.png"), QApplication::translate("MainWindow", "&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    // Create about Qt action and tie to MainWindow::aboutQt()
    aboutQtAct = new QAction(QIcon::fromTheme("info-about"), QApplication::translate("MainWindow", "About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // not for linux, because it has standart icons
#if !(defined (LINUX) || defined (__linux__)) || defined (ANDROID)
    openAct->setIcon(QIcon(":/icons/images/mobile/document-open.svg"));
    saveAct->setIcon(QIcon(":/icons/images/mobile/document-save.svg"));
    printAct->setIcon(QIcon(":/icons/images/mobile/document-print.svg"));
    previewAct->setIcon(QIcon(":/icons/images/mobile/document-print-preview.svg"));
    exitAct->setIcon(QIcon(":/icons/images/mobile/application-exit.svg"));
    penColorAct->setIcon(QIcon(":/icons/images/mobile/color-management.svg"));
    clearScreenAct->setIcon(QIcon(":/icons/images/mobile/edit-clear.svg"));
    penWidthAct->setIcon(QIcon(":/assets/images/pen-width-dark.png.png"));
    aboutQtAct->setIcon(QIcon(":/icons/images/mobile/help-about.svg"));
#endif
}

void MainWindow::clear() {
    // Check for changes since last save
    if (scribbleArea->isModified()) {
       QMessageBox::StandardButton ret;

       // Scribble is the title
       // Add text and the buttons
       ret = QMessageBox::warning(this, QApplication::translate("MainWindow", "Paint"),
                          QApplication::translate("MainWindow", "The image has been modified.\n"
                             "Do you want to save your changes?"),
                          QMessageBox::Save | QMessageBox::Discard
                          | QMessageBox::Cancel, QMessageBox::Save);

       // If save button clicked call for file to be saved
        if (ret == QMessageBox::Save) {
            save();

        // If cancel do nothing
        } else if (ret == QMessageBox::Cancel) {
            return;
        }
    }
    scribbleArea->clearImage();
}

// Create the menubar
void MainWindow::createMenus()
{
    // Create Save As option and the list of file types
    saveAsMenu = new QMenu(QApplication::translate("MainWindow", "&Save As"), this);
    foreach (QAction *action, saveAsActs)
        saveAsMenu->addAction(action);
    saveAsMenu->setIcon(QIcon::fromTheme("document-save-as"));

    // Attach all actions to File
    fileMenu = new QMenu(QApplication::translate("MainWindow", "&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addAction(printAct);
    fileMenu->addAction(previewAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    // Attach all actions to Options
    optionMenu = new QMenu(QApplication::translate("MainWindow", "&Options"), this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
    optionMenu->addSeparator();
    optionMenu->addAction(penAct);
    optionMenu->addSeparator();
#ifdef QT_DEBUG
    optionMenu->addAction(backColAct);
    optionMenu->addSeparator();
#endif
    optionMenu->addAction(clearScreenAct);

    // create language menu
    langMenu = new QMenu(QApplication::translate("MainWindow", "Language"), this);
    langMenu->addAction(germanAct);
    langMenu->addAction(englishAct);
    optionMenu->addSeparator();
    optionMenu->addMenu(langMenu);

    // Attach all actions to Help
    helpMenu = new QMenu(QApplication::translate("MainWindow", "&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    // Add menu items to the menubar
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(helpMenu);
}

void MainWindow::createToolbar() {
    toolbar = new QToolBar(this);
    toolbar->setObjectName("toolbar");

#ifdef Q_OS_MOBILE
    toolbar->setIconSize(QSize(128, 128));
    toolbar->setAllowedAreas(Qt::TopToolBarArea);
    toolbar->setMovable(false);
#else
    toolbar->setAllowedAreas(Qt::AllToolBarAreas);
    toolbar->setMovable(true);
#endif
    addToolBar(toolbar);
    toolbar->addAction(openAct);
    toolbar->addAction(saveAct);
#ifdef Q_OS_DESKTOP
    toolbar->addAction(printAct);
    toolbar->addAction(previewAct);
#endif
    toolbar->addAction(penColorAct);
    toolbar->addAction(penWidthAct);
    toolbar->addAction(penAct);
    toolbar->addAction(clearScreenAct);
}

void MainWindow::createStatusBar() {
    setStatusBar(statusBar());
    sizeLabel = new QLabel(this);
    sizeLabel->setAlignment(Qt::AlignCenter);
    sizeLabel->setStyleSheet("border: none");
    statusBar()->addPermanentWidget(sizeLabel, 1);
}

void MainWindow::updateStatusbar() {
    sizeLabel->setText(QString("%1X%2").arg(QString::number(scribbleArea->height()),
                                            QString::number(scribbleArea->width())));
}

bool MainWindow::maybeSave()
{
    // Check for changes since last save
    if (scribbleArea->isModified()) {
       QMessageBox::StandardButton ret;

       // Scribble is the title
       // Add text and the buttons
       ret = QMessageBox::warning(this, QApplication::translate("MainWindow", "Paint"),
                          QApplication::translate("MainWindow", "The image has been modified.\n"
                             "Do you want to save your changes?"),
                          QMessageBox::Save | QMessageBox::Discard
                          | QMessageBox::Cancel, QMessageBox::Save);

       // If save button clicked call for file to be saved
        if (ret == QMessageBox::Save) {
            return saveFile("png");

        // If cancel do nothing
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MainWindow::saveFile(const QByteArray &fileFormat)
{
    // Define path, name and default file type
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

    // Get selected file from dialog
    // Add the proper file formats and extensions
    QString fileName = QFileDialog::getSaveFileName(this, QApplication::translate("MainWindow", "Save As"),
                               initialPath,
                               QApplication::translate("MainWindow", "%1 Files (*.%2);;All Files (*)")
                               .arg(QString::fromLatin1(fileFormat.toUpper()),
                                    QString::fromLatin1(fileFormat)));

    // If no file do nothing
    if (fileName.isEmpty()) {
        return false;
    } else {
        path = fileName;
        // Call for the file to be saved
        return scribbleArea->saveImage(fileName, fileFormat.constData());
    }
}
