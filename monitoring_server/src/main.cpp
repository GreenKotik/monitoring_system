#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <QTextStream>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "monitoring_service.h"
#include "traymanager.h"
#include "database/dbmanager.h"
#include "utils/config.h"
#include "utils/logger.h"

int main(int argc, char *argv[])
{
    // ✅ Устанавливаем UTF-8 для консоли Windows
#ifdef Q_OS_WIN
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    QApplication app(argc, argv);
    app.setApplicationName("Monitoring Server");
    app.setApplicationVersion("1.0.0");
    app.setQuitOnLastWindowClosed(false);

    QTextStream out(stdout);

    out << "========================================" << Qt::endl;
    out << "   Monitoring Server v1.0.0" << Qt::endl;
    out << "========================================" << Qt::endl;
    out.flush();

    Logger::instance().init("monitoring_server.log");
    Logger::instance().info("Monitoring Server started");

    // Загрузка конфигурации
    out << "1. Loading config..." << Qt::endl;
    out.flush();

    if (!Config::instance().load("config.json")) {
        qCritical() << "Failed to load config.json";
        out << "   [ERROR] Failed to load config.json" << Qt::endl;
        out.flush();
        Logger::instance().error("Failed to load config.json");
        return 1;
    }

    out << "   [OK] Config loaded" << Qt::endl;
    out.flush();

    // Читаем параметры БД
    QString dbHost = Config::instance().get("database.host", "localhost").toString();
    int dbPort = Config::instance().get("database.port", 5432).toInt();
    QString dbName = Config::instance().get("database.name", "sensor_db").toString();
    QString dbUser = Config::instance().get("database.user", "postgres").toString();
    QString dbPassword = Config::instance().get("database.password", "12345").toString();

    out << "2. Connecting to PostgreSQL database..." << Qt::endl;
    out.flush();
    out << "   Host: " << dbHost << Qt::endl;
    out << "   Port: " << dbPort << Qt::endl;
    out << "   Database: " << dbName << Qt::endl;
    out << "   User: " << dbUser << Qt::endl;
    out.flush();

    bool dbConnected = DbManager::instance().initialize(
        "QPSQL", dbName, dbHost, dbPort, dbUser, dbPassword
    );

    if (!dbConnected) {
        out << "   [ERROR] " << DbManager::instance().lastError() << Qt::endl;
        out.flush();
        Logger::instance().error("Database connection failed: " + DbManager::instance().lastError());
        return 1;
    }

    out << "   [OK] PostgreSQL connected successfully" << Qt::endl;
    out.flush();

    // Создаём менеджер системного трея
    TrayManager trayManager;

    if (TrayManager::isAvailable()) {
        trayManager.show();
        trayManager.showMessage(
            "Monitoring Server",
            "Сервер мониторинга запущен и работает в фоновом режиме.",
            QSystemTrayIcon::Information,
            3000
        );
        out << "   [OK] System tray icon created" << Qt::endl;
        out.flush();
        Logger::instance().info("System tray icon created");
    } else {
        out << "   [WARN] System tray is not available" << Qt::endl;
        out.flush();
        Logger::instance().warning("System tray is not available on this system");
    }

    // Подключаем сигнал "Завершить" из трея
    QObject::connect(&trayManager, &TrayManager::quitRequested, [&]() {
        out << "========================================" << Qt::endl;
        out << "   Quit requested from tray icon" << Qt::endl;
        out << "========================================" << Qt::endl;
        out.flush();

        Logger::instance().info("Quit requested from tray icon");

        trayManager.showMessage(
            "Monitoring Server",
            "Сервер мониторинга завершает работу...",
            QSystemTrayIcon::Information,
            2000
        );

        QTimer::singleShot(1000, &app, &QApplication::quit);
    });

    // Создаём и запускаем MonitoringService
    out << "3. Creating and starting MonitoringService..." << Qt::endl;
    out.flush();

    MonitoringService service;

    if (!service.initialize()) {
        qCritical() << "Failed to initialize monitoring service";
        out << "   [ERROR] Failed to initialize monitoring service" << Qt::endl;
        out.flush();
        Logger::instance().error("Failed to initialize monitoring service");
        return 1;
    }

    service.start();

    out << "   [OK] MonitoringService started" << Qt::endl;
    out.flush();
    out << "========================================" << Qt::endl;
    out << "   Monitoring Server is running" << Qt::endl;
    out << "   Right-click tray icon to quit" << Qt::endl;
    out << "========================================" << Qt::endl;
    out.flush();

    Logger::instance().info("Monitoring Server running...");

    return app.exec();
}



/*
#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>
#include "monitoring_service.h"
#include "database/dbmanager.h"
#include "utils/config.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);

    out << "========================================" << Qt::endl;
    out << "   Monitoring Server v1.0.0" << Qt::endl;
    out << "========================================" << Qt::endl;
    out.flush();

    out << "1. Loading config..." << Qt::endl;
    out.flush();

    if (!Config::instance().load("config.json")) {
        qCritical() << "Failed to load config.json";
        out << "   ❌ Failed to load config.json" << Qt::endl;
        out.flush();
        return 1;
    }
    out << "   ✅ Config loaded" << Qt::endl;
    out.flush();

    // Читаем параметры БД из конфига
    QString dbHost = Config::instance().get("database.host", "localhost").toString();
    int dbPort = Config::instance().get("database.port", 5432).toInt();
    QString dbName = Config::instance().get("database.name", "sensor_db").toString();  // <-- sensor_db по умолчанию
    QString dbUser = Config::instance().get("database.user", "postgres").toString();
    QString dbPassword = Config::instance().get("database.password", "12345").toString();

    out << "2. Connecting to PostgreSQL database..." << Qt::endl;
    out.flush();
    out << "   Host: " << dbHost << Qt::endl;
    out << "   Port: " << dbPort << Qt::endl;
    out << "   Database: " << dbName << Qt::endl;
    out << "   User: " << dbUser << Qt::endl;
    out.flush();

    bool dbConnected = DBManager::instance().initialize(
        "QPSQL",
        dbName,
        dbHost,
        dbPort,
        dbUser,
        dbPassword
    );

    if (!dbConnected) {
        out << "   ❌ ERROR: " << DBManager::instance().lastError() << Qt::endl;
        out.flush();
        return 1;
    }

    out << "   ✅ PostgreSQL connected successfully" << Qt::endl;
    out.flush();

    out << "3. Creating and starting MonitoringService..." << Qt::endl;
    out.flush();

    MonitoringService service;

    if (!service.initialize()) {
        qCritical() << "Failed to initialize monitoring service";
        out << "   ❌ Failed to initialize monitoring service" << Qt::endl;
        out.flush();
        return 1;
    }

    service.start();

    out << "   ✅ MonitoringService started" << Qt::endl;
    out.flush();

    out << "========================================" << Qt::endl;
    out << "   Monitoring Server is running" << Qt::endl;
    out << "   Press Ctrl+C to stop" << Qt::endl;
    out << "========================================" << Qt::endl;
    out.flush();

    return a.exec();
}
*/
/*
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include "collectors/snmpcollector.h"
#include "core/database/dbmanager.h"
#include "core/utils/logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Logger::instance().init("monitoring_server.log");
    Logger::instance().info("Monitoring Server started");

    // Подключение к БД
    if (!DbManager::instance().connect("localhost", 5432, "monitoring", "postgres", "password")) {
        Logger::instance().error("Failed to connect to database");
        return 1;
    }

    // Создаем SNMP коллектор
    SnmpCollector snmpCollector;

    // Подключаем сигнал к слоту для записи в БД
    QObject::connect(&snmpCollector, &SnmpCollector::dataReceived,
        [](const QString &sensorId, qreal value) {
            Logger::instance().info(QString("Data received: %1 = %2").arg(sensorId).arg(value));
            DbManager::instance().addReading(sensorId, value);
        });

    // Подключаем обработку ошибок
    QObject::connect(&snmpCollector, &SnmpCollector::errorOccurred,
        [](const QString &deviceId, const QString &error) {
            Logger::instance().error(QString("SNMP error on %1: %2").arg(deviceId).arg(error));
        });

    // Настраиваем устройство
    SnmpCollector::SnmpDevice device;
    device.deviceId = "router_1";
    device.host = "192.168.1.1";
    device.port = 161;
    device.community = "public";
    device.timeoutMs = 2000;

    // Добавляем OID для опроса
    device.oids["cpu_usage"] = "1.3.6.1.4.1.9.9.109.1.1.1.1.5.1";
    device.oids["memory_usage"] = "1.3.6.1.4.1.9.9.48.1.1.1.5.1";
    device.oids["temperature"] = "1.3.6.1.4.1.9.9.13.1.3.1.3.1";

    snmpCollector.addDevice(device);

    // Запускаем опрос каждые 5 секунд
    snmpCollector.start(5000);

    Logger::instance().info("Monitoring Server running...");

    return app.exec();
}
*/
