#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include "httpserver.h"
#include "httprouter.h"
#include "controllers/authcontroller.h"
#include "controllers/objectcontroller.h"
#include "controllers/sensorcontroller.h"
#include "controllers/mapcontroller.h"
#include "database/dbmanager.h"
#include "utils/config.h"
#include <QSqlDatabase>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);

    out << "========================================" << Qt::endl;
    out << "   App Server v1.0.0" << Qt::endl;
    out << "========================================" << Qt::endl;

    // ДОБАВЛЯЕМ: устанавливаем путь к плагинам Qt
    QString pluginPath = QCoreApplication::applicationDirPath() + "/../sqldrivers";

    // Пробуем несколько вариантов путей
    QStringList pluginPaths;
    pluginPaths << QCoreApplication::applicationDirPath() + "/sqldrivers";
    pluginPaths << QCoreApplication::applicationDirPath() + "/../plugins/sqldrivers";
    pluginPaths << "C:/Qt/5.15.2/mingw81_64/plugins/sqldrivers";  // Путь к плагинам Qt

    // Ищем существующий путь
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
    } else {
        out << "Plugin path not found, using default" << Qt::endl;
    }

    // Выводим все пути для диагностики
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

    // Выводим доступные драйверы для диагностики
    out << "Available SQL drivers:" << QSqlDatabase::drivers().join(", ") << Qt::endl;
    out.flush();

    bool dbConnected = DbManager::instance().initialize(
        "QPSQL",
        Config::instance().get("database.name", "sensor_db").toString(),
        Config::instance().get("database.host", "localhost").toString(),
        Config::instance().get("database.port", 5432).toInt(),
        Config::instance().get("database.user", "postgres").toString(),
        Config::instance().get("database.password", "12345").toString()
    );

    if (!dbConnected) {
        qCritical() << "Failed to connect to database:" << DbManager::instance().lastError();
        out << "   ❌ ERROR: " << DbManager::instance().lastError() << Qt::endl;

        // Пробуем SQLite как fallback
        out << "   Trying SQLite as fallback..." << Qt::endl;
        dbConnected = DbManager::instance().initialize(
            "QSQLITE",
            "monitoring.db",
            "",
            0,
            "",
            ""
        );

        if (!dbConnected) {
            qCritical() << "Failed to connect to SQLite:" << DbManager::instance().lastError();
            return 1;
        }
        out << "   ✅ SQLite connected successfully" << Qt::endl;
    } else {
        out << "   ✅ PostgreSQL connected successfully" << Qt::endl;
    }

    // Создаем маршруты
    HttpRouter router;

    // Публичные маршруты
    router.addRoute("POST", "/api/auth/login", AuthController::login);
    router.addRoute("POST", "/api/auth/signup", AuthController::signup);

    // Защищенные маршруты
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
    out << "   Press Ctrl+C to stop" << Qt::endl;
    out << "========================================" << Qt::endl;
    out.flush();

    return a.exec();
}
