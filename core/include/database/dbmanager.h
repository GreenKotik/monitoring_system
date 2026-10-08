#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include <QString>
#include "../core_global.h"
#include "../models/object.h"
#include "../models/objecttype.h"      // <-- ДОБАВИТЬ ЭТУ СТРОКУ
#include "../models/sensor.h"
#include "../models/sensortype.h"      // <-- ДОБАВИТЬ ЭТУ СТРОКУ
#include "../models/sensorreading.h"

// ============================================================================
// СТРУКТУРЫ ДЛЯ ДИНАМИЧЕСКОЙ КОНФИГУРАЦИИ
// ============================================================================
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
// ============================================================================

class CORE_EXPORT DbManager
{
public:
    static DbManager& instance();

    // Оригинальный метод подключения
    bool connect(const QString &host, int port, const QString &database,
                 const QString &user, const QString &password);
    void disconnect();

    // Эти методы реализованы прямо здесь (inline), поэтому ошибок линковки не будет
    bool isConnected() const { return m_db.isOpen(); }
    QString lastError() const { return m_lastError; }

    // CRUD для объектов
    QList<Object> getObjects(const QString &parentId = QString());
    QList<Object> getRootObjects();
    Object getObject(const QString &objectId);
    bool createObject(const Object &object);
    bool updateObject(const Object &object);
    bool deleteObject(const QString &objectId);

    // CRUD для датчиков
    QList<Sensor> getSensors(const QString &objectId = QString());
    Sensor getSensor(const QString &sensorId);
    bool createSensor(const Sensor &sensor);
    bool updateSensor(const Sensor &sensor);
    bool deleteSensor(const QString &sensorId);

    // Показания датчиков
    bool addReading(const QString &sensorId, qreal value);
    QList<SensorReading> getHistory(const QString &sensorId, int count = 100);
    QList<SensorReading> getHistory(const QString &sensorId, const QDateTime &from, const QDateTime &to);

    // Типы
    QList<ObjectType> getObjectTypes();
    QList<SensorType> getSensorTypes();

    // Транзакции
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    // Миграции
    bool runMigrations();

    // --- НОВЫЕ МЕТОДЫ ДЛЯ ЧТЕНИЯ КОНФИГУРАЦИИ ---
    QList<SensorConfig> getActiveSensors();
    QList<DeviceConfig> getActiveDeviceConfigs();
    // ---------------------------------------------

private:
    DbManager() = default; // <-- ВАЖНО: без QObject*, без параметров
    ~DbManager();
    DbManager(const DbManager&) = delete;
    DbManager& operator=(const DbManager&) = delete;

    QSqlQuery executeQuery(const QString &query, const QVariantList &params = {});

    QSqlDatabase m_db;
    QString m_lastError;
};

#endif // DBMANAGER_H
