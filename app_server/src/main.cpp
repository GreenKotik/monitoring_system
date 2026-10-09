#include <QApplication>  // ✅ ЗАМЕНЕНО: было QCoreApplication
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include <QTimer>        // ✅ ДОБАВЛЕНО: для задержки перед выходом
#include "httpserver.h"
#include "httprouter.h"
#include "controllers/authcontroller.h"
#include "controllers/objectcontroller.h"
#include "controllers/sensorcontroller.h"
#include "controllers/mapcontroller.h"
#include "database/dbmanager.h"
#include "utils/config.h"
#include <QSqlDatabase>
#include "traymanager.h" // ✅ ДОБАВЛЕНО

int main(int argc, char *argv[])
{
    // ✅ ЗАМЕНЕНО: QApplication вместо QCoreApplication
    QApplication a(argc, argv);
    a.setApplicationName("App Server");
    a.setApplicationVersion("1.0.0");
    a.setQuitOnLastWindowClosed(false); // ✅ ВАЖНО: не закрывать приложение, так как окон нет

    QTextStream out(stdout);
    out << "========================================" << Qt::endl;
    out << "   App Server v1.0.0" << Qt::endl;
    out << "========================================" << Qt::endl;

    // Установка пути к плагинам Qt (ваша существующая логика)
    QStringList pluginPaths;
    pluginPaths << QCoreApplication::applicationDirPath() + "/sqldrivers";
    pluginPaths << QCoreApplication::applicationDirPath() + "/../plugins/sqldrivers";
    pluginPaths << "C:/Qt/5.15.2/mingw81_64/plugins/sqldrivers";

    QString foundPath;
    for (const QString &path : pluginPaths) {
        QDir dir(path);
        if (dir.exists()) {
            foundPath = path;
            break;
        }
    }

    if (!foundPath.isEmpty()) {
        out << "Adding plugin path: " << foundPath << Qt::endl;
        QCoreApplication::addLibraryPath(foundPath);
    }

    out << "Library paths:" << Qt::endl;
    for (const QString &path : QCoreApplication::libraryPaths()) {
        out << "  " << path << Qt::endl;
    }

    if (!Config::instance().load("config.json")) {
        qCritical() << "Failed to load config.json";
        return 1;
    }

    out << "Connecting to PostgreSQL database..." << Qt::endl;
    out.flush();
    out << "Available SQL drivers:" << QSqlDatabase::drivers().join(", ") << Qt::endl;
    out.flush();

    // ✅ ИСПРАВЛЕНО: используем connect() с 5 параметрами (без driver)
    bool dbConnected = DbManager::instance().connect(
        Config::instance().get("database.host", "localhost").toString(),
        Config::instance().get("database.port", 5432).toInt(),
        Config::instance().get("database.name", "sensor_db").toString(),
        Config::instance().get("database.user", "postgres").toString(),
        Config::instance().get("database.password", "12345").toString()
    );

    if (!dbConnected) {
        qCritical() << "Failed to connect to database:" << DbManager::instance().lastError();
        out << "   [ERROR] " << DbManager::instance().lastError() << Qt::endl;
        return 1;
    } else {
        out << "   [OK] PostgreSQL connected successfully" << Qt::endl;
    }

    // ========================================================================
    // ✅ ДОБАВЛЕНО: Системный трей
    // ========================================================================
    // ✅ Синяя иконка с буквой "A" (App/API)
    TrayManager trayManager("App Server", "A", QColor(33, 150, 243));

    if (TrayManager::isAvailable()) {
        trayManager.show();
        trayManager.showMessage(
            "App Server",
            "REST API сервер запущен и работает в фоновом режиме.",
            QSystemTrayIcon::Information,
            3000
        );
        out << "   [OK] System tray icon created" << Qt::endl;
    } else {
        out << "   [WARN] System tray is not available" << Qt::endl;
    }

    // Подключаем сигнал "Завершить" из трея
    QObject::connect(&trayManager, &TrayManager::quitRequested, [&]() {
        out << "========================================" << Qt::endl;
        out << "   Quit requested from tray icon" << Qt::endl;
        out << "========================================" << Qt::endl;
        out.flush();

        trayManager.showMessage(
            "App Server",
            "Сервер завершает работу...",
            QSystemTrayIcon::Information,
            2000
        );

        // Даём 1 секунду на отображение уведомления перед выходом
        QTimer::singleShot(1000, &a, &QApplication::quit);
    });
    // ========================================================================

    // Создаем маршруты (ваша существующая логика)
    HttpRouter router;
    router.addRoute("POST", "/api/auth/login", AuthController::login);
    router.addRoute("POST", "/api/auth/signup", AuthController::signup);
    router.addRoute("GET", "/api/objects", ObjectController::list);
    router.addRoute("GET", "/api/objects/:id", ObjectController::get);
    router.addRoute("POST", "/api/objects", ObjectController::create);
    router.addRoute("PUT", "/api/objects/:id", ObjectController::update);
    router.addRoute("DELETE", "/api/objects/:id", ObjectController::remove);
    router.addRoute("GET", "/api/sensors", SensorController::list);
    router.addRoute("GET", "/api/sensors/:id", SensorController::get);
    router.addRoute("GET", "/api/sensors/:id/history", SensorController::getHistory);
    router.addRoute("POST", "/api/sensors", SensorController::create);
    router.addRoute("PUT", "/api/sensors/:id", SensorController::update);
    router.addRoute("DELETE", "/api/sensors/:id", SensorController::remove);
    router.addRoute("GET", "/api/map/:id", MapController::getMap);
    router.addRoute("GET", "/api/scheme/:id", MapController::getScheme);

    // Запускаем HTTP сервер
    int port = Config::instance().get("server.port", 8080).toInt();
    HttpServer server;
    server.setRouter(&router);
    if (!server.start(port)) {
        qCritical() << "Failed to start HTTP server on port:" << port;
        return 1;
    }

    out << "========================================" << Qt::endl;
    out << "   App Server started successfully" << Qt::endl;
    out << "   Port: " << port << Qt::endl;
    out << "   Right-click tray icon to quit" << Qt::endl; // ✅ Обновлено подсказка
    out << "========================================" << Qt::endl;
    out.flush();

    return a.exec();
}

