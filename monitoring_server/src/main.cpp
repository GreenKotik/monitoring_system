#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include "collectors/snmpcollector.h"
#include "collectors/modbuscollector.h"
#include "collectors/mqttcollector.h"
#include "core/database/dbmanager.h"
#include "core/utils/logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Logger::instance().init("monitoring_server.log");
    Logger::instance().info("Monitoring Server started");

    // Подключение к БД
    if (!DbManager::instance().connect("localhost", 5432, "monitoring", "postgres", "password")) {
        Logger::instance().error("Failed to connect to database: " + DbManager::instance().lastError());
        return 1;
    }

    // SNMP коллектор
    SnmpCollector snmpCollector;
    SnmpCollector::SnmpDevice device;
    device.deviceId = "device_1";
    device.host = "192.168.1.100";
    device.community = "public";
    device.oids["temp_101"] = "1.3.6.1.2.1.1.1.0";
    snmpCollector.addDevice(device);

    QObject::connect(&snmpCollector, &SnmpCollector::dataReceived,
        [](const QString &sensorId, qreal value) {
            Logger::instance().info(QString("Data received: %1 = %2").arg(sensorId).arg(value));
            DbManager::instance().addReading(sensorId, value);
        });

    snmpCollector.start(5000);

    Logger::instance().info("Monitoring Server running...");

    return app.exec();
}