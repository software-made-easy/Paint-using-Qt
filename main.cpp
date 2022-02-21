#include "Paint.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    // The main application
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName("SME");
    app.setOrganizationDomain("https://github.com/software-made-easy/Paint-using-Qt");

    // Create and open the main window
    MainWindow window;
    window.show();

    // Display the main window
    return app.exec();
}
