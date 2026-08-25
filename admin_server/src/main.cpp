#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include "admin_mainwindow.h"
#include "core/utils/logger.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Admin Server");
    app.setOrganizationName("MonitoringSystem");
    app.setApplicationVersion("1.0.0");

    // Загрузка стилей
    QFile styleFile(":/styles/dark_style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    } else {
        app.setStyle(QStyleFactory::create("Fusion"));
    }

    Logger::instance().init("admin_server.log");
    Logger::instance().info("Admin Server started");

    AdminMainWindow window;
    window.show();

    return app.exec();
}