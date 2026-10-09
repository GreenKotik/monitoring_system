#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Monitoring System");
    app.setOrganizationName("YourCompany");
    app.setApplicationVersion("1.0.0");

    // Загрузка стилей
    QFile styleFile(":/styles/dark_style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    } else {
        app.setStyle(QStyleFactory::create("Fusion"));
    }

    MainWindow window;
    window.showMaximized();  // ← открывает окно на весь экран
    return app.exec();
}
