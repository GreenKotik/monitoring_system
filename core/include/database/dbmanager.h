#pragma once

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QDateTime>
#include <QJsonObject>
#include "models/object.h"
#include "models/sensor.h"
#include "models/sensorreading.h"
#include "models/user.h"
#include <QMap>

struct SensorConfig {
    QString sensorId;
    QString name;
    QString protocol;
    QString host;
    int port;
    QString community;
    QString oid;
    int modbusAddress;
    int modbusRegister;
    int pollIntervalMs;
    bool enabled;
    QString command;         // Для TCP
    QString terminator;      // Для TCP (CR, LF, CRLF)
    QString regex;           // Для TCP (регулярное выражение)
    QString checksumType;    // Для TCP (None, XOR, CRC16)
};

struct DeviceConfig {
    QString deviceId;
    QString host;
    int port;
    QString protocol;
    QString community;
    int timeoutMs;
    QList<SensorConfig> sensors;
};

class DbManager : public QObject
{
    Q_OBJECT

private:
    explicit DbManager(QObject *parent = nullptr);
    ~DbManager();

public:
    static DbManager& instance();
    bool initialize(const QString &driver = "QSQLITE",
                    const QString &database = "monitoring.db",
                    const QString &host = "",
                    int port = 0,
                    const QString &user = "",
                    const QString &password = "");
    void disconnect();
    bool isOpen() const;
    QString lastError() const;

    // CRUD для Object - УБИРАЕМ const
    bool createObject(const Object &object);
    QList<Object> getObjects(const QString &parentId = QString());
    Object getObjectById(const QString &id);
    bool updateObject(const Object &object);
    bool deleteObject(const QString &id);

    // CRUD для Sensor - УБИРАЕМ const
    bool createSensor(const Sensor &sensor);
    QList<Sensor> getSensors(const QString &objectId = QString());
    Sensor getSensorById(const QString &id);
    bool updateSensor(const Sensor &sensor);
    bool deleteSensor(const QString &id);

    // Чтения - УБИРАЕМ const
    bool addReading(const QString &sensorId, double value);
    QList<SensorReading> getHistory(const QString &sensorId, int count = 100);
    QList<SensorReading> getHistory(const QString &sensorId,
                                    const QDateTime &from,
                                    const QDateTime &to);

    // Методы для загрузки конфигурации
    QList<DeviceConfig> getActiveDeviceConfigs();
    QList<SensorConfig> getActiveSensors();
    // Миграции
    bool runMigrations();

private:
    QSqlQuery executeQuery(const QString &query, const QVariantList &params = QVariantList());
    Object parseObject(const QSqlQuery &query) const;
    Sensor parseSensor(const QSqlQuery &query) const;
    SensorReading parseReading(const QSqlQuery &query) const;

    QSqlDatabase m_db;
    QString m_lastError;
    bool m_initialized;
};
