#include "database/dbmanager.h"
#include "database/dbmigrations.h"
#include "utils/logger.h"
#include <QSqlRecord>
#include <QVariant>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

DbManager& DbManager::instance()
{
    static DbManager instance;
    return instance;
}

DbManager::~DbManager()
{
    disconnect();
}

bool DbManager::connect(const QString &host, int port, const QString &database,
                        const QString &user, const QString &password)
{
    if (m_db.isOpen()) {
        return true;
    }

    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(database);
    m_db.setUserName(user);
    m_db.setPassword(password);

    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        Logger::instance().error("Database connection failed: " + m_lastError);
        return false;
    }

    Logger::instance().info("Database connected successfully");

    if (!runMigrations()) {
        Logger::instance().error("Database migrations failed");
        return false;
    }

    return true;
}

void DbManager::disconnect()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

QSqlQuery DbManager::executeQuery(const QString &query, const QVariantList &params)
{
    if (!m_db.isOpen()) {
        Logger::instance().error("Database not connected");
        return QSqlQuery();
    }

    QSqlQuery sqlQuery(m_db);
    if (!sqlQuery.prepare(query)) {
        m_lastError = sqlQuery.lastError().text();
        Logger::instance().error("Query prepare failed: " + m_lastError);
        return sqlQuery;
    }

    for (int i = 0; i < params.size(); ++i) {
        sqlQuery.bindValue(i, params[i]);
    }

    if (!sqlQuery.exec()) {
        m_lastError = sqlQuery.lastError().text();
        Logger::instance().error("Query execution failed: " + m_lastError);
        return sqlQuery;
    }

    return sqlQuery;
}

QList<Object> DbManager::getObjects(const QString &parentId)
{
    QList<Object> objects;
    QString query = "SELECT * FROM objects";
    QVariantList params;

    if (!parentId.isEmpty()) {
        query += " WHERE parent_object_id = ?";
        params.append(parentId);
    }

    query += " ORDER BY name";

    QSqlQuery sqlQuery = executeQuery(query, params);
    if (!sqlQuery.isActive()) return objects;

    while (sqlQuery.next()) {
        Object obj;
        obj.setId(sqlQuery.value("id").toString());
        obj.setName(sqlQuery.value("name").toString());
        obj.setTypeId(sqlQuery.value("type_id").toString());
        obj.setTypeName(sqlQuery.value("type_name").toString());
        obj.setParentObjectId(sqlQuery.value("parent_object_id").toString());
        obj.setDescription(sqlQuery.value("description").toString());
        obj.setPositionX(sqlQuery.value("position_x").toDouble());
        obj.setPositionY(sqlQuery.value("position_y").toDouble());
        obj.setSizeWidth(sqlQuery.value("size_width").toDouble());
        obj.setSizeHeight(sqlQuery.value("size_height").toDouble());
        obj.setColorCode(sqlQuery.value("color_code").toString());
        obj.setSvgSchemePath(sqlQuery.value("svg_scheme_path").toString());
        obj.setIsActive(sqlQuery.value("is_active").toBool());
        obj.setCreatedAt(sqlQuery.value("created_at").toDateTime());
        obj.setUpdatedAt(sqlQuery.value("updated_at").toDateTime());
        objects.append(obj);
    }

    return objects;
}

QList<Object> DbManager::getRootObjects()
{
    return getObjects("");
}

Object DbManager::getObject(const QString &objectId)
{
    QList<Object> objects = getObjects();
    for (const Object &obj : objects) {
        if (obj.id() == objectId) {
            return obj;
        }
    }
    return Object();
}

bool DbManager::createObject(const Object &object)
{
    QString query = "INSERT INTO objects (id, name, type_id, type_name, parent_object_id, "
                    "description, position_x, position_y, size_width, size_height, "
                    "color_code, svg_scheme_path, is_active) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    QVariantList params;
    params.append(object.id());
    params.append(object.name());
    params.append(object.typeId());
    params.append(object.typeName());
    params.append(object.parentObjectId());
    params.append(object.description());
    params.append(object.positionX());
    params.append(object.positionY());
    params.append(object.sizeWidth());
    params.append(object.sizeHeight());
    params.append(object.colorCode());
    params.append(object.svgSchemePath());
    params.append(object.isActive());

    QSqlQuery sqlQuery = executeQuery(query, params);
    return sqlQuery.isActive();
}

bool DbManager::updateObject(const Object &object)
{
    QString query = "UPDATE objects SET name = ?, type_id = ?, type_name = ?, "
                    "parent_object_id = ?, description = ?, position_x = ?, position_y = ?, "
                    "size_width = ?, size_height = ?, color_code = ?, svg_scheme_path = ?, "
                    "is_active = ?, updated_at = NOW() WHERE id = ?";

    QVariantList params;
    params.append(object.name());
    params.append(object.typeId());
    params.append(object.typeName());
    params.append(object.parentObjectId());
    params.append(object.description());
    params.append(object.positionX());
    params.append(object.positionY());
    params.append(object.sizeWidth());
    params.append(object.sizeHeight());
    params.append(object.colorCode());
    params.append(object.svgSchemePath());
    params.append(object.isActive());
    params.append(object.id());

    QSqlQuery sqlQuery = executeQuery(query, params);
    return sqlQuery.isActive() && sqlQuery.numRowsAffected() > 0;
}

bool DbManager::deleteObject(const QString &objectId)
{
    QString query = "DELETE FROM objects WHERE id = ?";
    QVariantList params;
    params.append(objectId);

    QSqlQuery sqlQuery = executeQuery(query, params);
    return sqlQuery.isActive() && sqlQuery.numRowsAffected() > 0;
}

QList<Sensor> DbManager::getSensors(const QString &objectId)
{
    QList<Sensor> sensors;
    QString query = "SELECT * FROM sensors";
    QVariantList params;

    if (!objectId.isEmpty()) {
        query += " WHERE object_id = ?";
        params.append(objectId);
    }

    query += " ORDER BY name";

    QSqlQuery sqlQuery = executeQuery(query, params);
    if (!sqlQuery.isActive()) return sensors;

    while (sqlQuery.next()) {
        Sensor sensor;
        sensor.setId(sqlQuery.value("id").toString());
        sensor.setName(sqlQuery.value("name").toString());
        sensor.setTypeId(sqlQuery.value("type_id").toString());
        sensor.setTypeName(sqlQuery.value("type_name").toString());
        sensor.setObjectId(sqlQuery.value("object_id").toString());
        sensor.setObjectName(sqlQuery.value("object_name").toString());
        sensor.setUnit(sqlQuery.value("unit").toString());
        sensor.setMinValue(sqlQuery.value("min_value").toDouble());
        sensor.setMaxValue(sqlQuery.value("max_value").toDouble());
        sensor.setLastValue(sqlQuery.value("last_value").toDouble());
        sensor.setStatus(sqlQuery.value("status").toString());
        sensor.setPollingInterval(sqlQuery.value("polling_interval").toInt());
        sensor.setConnectionParams(sqlQuery.value("connection_params").toString());
        sensor.setLastUpdate(sqlQuery.value("last_update").toDateTime());
        sensor.setCreatedAt(sqlQuery.value("created_at").toDateTime());
        sensor.setIsActive(sqlQuery.value("is_active").toBool());
        sensors.append(sensor);
    }

    return sensors;
}

Sensor DbManager::getSensor(const QString &sensorId)
{
    QList<Sensor> sensors = getSensors();
    for (const Sensor &sensor : sensors) {
        if (sensor.id() == sensorId) {
            return sensor;
        }
    }
    return Sensor();
}

bool DbManager::createSensor(const Sensor &sensor)
{
    QString query = "INSERT INTO sensors (id, name, type_id, type_name, object_id, object_name, "
                    "unit, min_value, max_value, last_value, status, polling_interval, "
                    "connection_params, is_active) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    QVariantList params;
    params.append(sensor.id());
    params.append(sensor.name());
    params.append(sensor.typeId());
    params.append(sensor.typeName());
    params.append(sensor.objectId());
    params.append(sensor.objectName());
    params.append(sensor.unit());
    params.append(sensor.minValue());
    params.append(sensor.maxValue());
    params.append(sensor.lastValue());
    params.append(sensor.status());
    params.append(sensor.pollingInterval());
    params.append(sensor.connectionParams());
    params.append(sensor.isActive());

    QSqlQuery sqlQuery = executeQuery(query, params);
    return sqlQuery.isActive();
}

bool DbManager::updateSensor(const Sensor &sensor)
{
    QString query = "UPDATE sensors SET name = ?, type_id = ?, type_name = ?, "
                    "object_id = ?, object_name = ?, unit = ?, min_value = ?, max_value = ?, "
                    "last_value = ?, status = ?, polling_interval = ?, connection_params = ?, "
                    "is_active = ?, last_update = NOW() WHERE id = ?";

    QVariantList params;
    params.append(sensor.name());
    params.append(sensor.typeId());
    params.append(sensor.typeName());
    params.append(sensor.objectId());
    params.append(sensor.objectName());
    params.append(sensor.unit());
    params.append(sensor.minValue());
    params.append(sensor.maxValue());
    params.append(sensor.lastValue());
    params.append(sensor.status());
    params.append(sensor.pollingInterval());
    params.append(sensor.connectionParams());
    params.append(sensor.isActive());
    params.append(sensor.id());

    QSqlQuery sqlQuery = executeQuery(query, params);
    return sqlQuery.isActive() && sqlQuery.numRowsAffected() > 0;
}

bool DbManager::deleteSensor(const QString &sensorId)
{
    QString query = "DELETE FROM sensors WHERE id = ?";
    QVariantList params;
    params.append(sensorId);

    QSqlQuery sqlQuery = executeQuery(query, params);
    return sqlQuery.isActive() && sqlQuery.numRowsAffected() > 0;
}

bool DbManager::addReading(const QString &sensorId, qreal value)
{
    QString query = "INSERT INTO sensor_readings (sensor_id, value) VALUES (?, ?)";
    QVariantList params;
    params.append(sensorId);
    params.append(value);

    QSqlQuery sqlQuery = executeQuery(query, params);
    return sqlQuery.isActive();
}

QList<SensorReading> DbManager::getHistory(const QString &sensorId, int count)
{
    QList<SensorReading> history;
    QString query = "SELECT * FROM sensor_readings "
                    "WHERE sensor_id = ? "
                    "ORDER BY timestamp DESC LIMIT ?";

    QVariantList params;
    params.append(sensorId);
    params.append(count);

    QSqlQuery sqlQuery = executeQuery(query, params);
    if (!sqlQuery.isActive()) return history;

    while (sqlQuery.next()) {
        SensorReading reading;
        reading.setId(sqlQuery.value("id").toString());
        reading.setSensorId(sqlQuery.value("sensor_id").toString());
        reading.setValue(sqlQuery.value("value").toDouble());
        reading.setQuality(sqlQuery.value("quality").toString());
        reading.setTimestamp(sqlQuery.value("timestamp").toDateTime());
        history.append(reading);
    }

    return history;
}

QList<SensorReading> DbManager::getHistory(const QString &sensorId,
                                          const QDateTime &from,
                                          const QDateTime &to)
{
    QList<SensorReading> history;
    QString query = "SELECT * FROM sensor_readings "
                    "WHERE sensor_id = ? AND timestamp BETWEEN ? AND ? "
                    "ORDER BY timestamp ASC";

    QVariantList params;
    params.append(sensorId);
    params.append(from);
    params.append(to);

    QSqlQuery sqlQuery = executeQuery(query, params);
    if (!sqlQuery.isActive()) return history;

    while (sqlQuery.next()) {
        SensorReading reading;
        reading.setId(sqlQuery.value("id").toString());
        reading.setSensorId(sqlQuery.value("sensor_id").toString());
        reading.setValue(sqlQuery.value("value").toDouble());
        reading.setQuality(sqlQuery.value("quality").toString());
        reading.setTimestamp(sqlQuery.value("timestamp").toDateTime());
        history.append(reading);
    }

    return history;
}

QList<ObjectType> DbManager::getObjectTypes()
{
    QList<ObjectType> types;
    QString query = "SELECT * FROM object_types ORDER BY type_name";

    QSqlQuery sqlQuery = executeQuery(query);
    if (!sqlQuery.isActive()) return types;

    while (sqlQuery.next()) {
        ObjectType type;
        type.setTypeId(sqlQuery.value("type_id").toInt());
        type.setTypeCode(sqlQuery.value("type_code").toString());
        type.setTypeName(sqlQuery.value("type_name").toString());
        type.setCanHaveChildren(sqlQuery.value("can_have_children").toBool());
        type.setIconName(sqlQuery.value("icon_name").toString());
        type.setColorCode(sqlQuery.value("color_code").toString());
        type.setSvgIconPath(sqlQuery.value("svg_icon_path").toString());
        types.append(type);
    }

    return types;
}

QList<SensorType> DbManager::getSensorTypes()
{
    QList<SensorType> types;
    QString query = "SELECT * FROM sensor_types ORDER BY type_name";

    QSqlQuery sqlQuery = executeQuery(query);
    if (!sqlQuery.isActive()) return types;

    while (sqlQuery.next()) {
        SensorType type;
        type.setTypeId(sqlQuery.value("type_id").toInt());
        type.setTypeCode(sqlQuery.value("type_code").toString());
        type.setTypeName(sqlQuery.value("type_name").toString());
        type.setUnit(sqlQuery.value("unit").toString());
        type.setColorCode(sqlQuery.value("color_code").toString());
        type.setIconName(sqlQuery.value("icon_name").toString());
        type.setSvgImagePath(sqlQuery.value("svg_image_path").toString());
        types.append(type);
    }

    return types;
}

bool DbManager::beginTransaction()
{
    if (!m_db.isOpen()) return false;
    return m_db.transaction();
}

bool DbManager::commitTransaction()
{
    if (!m_db.isOpen()) return false;
    return m_db.commit();
}

bool DbManager::rollbackTransaction()
{
    if (!m_db.isOpen()) return false;
    return m_db.rollback();
}

bool DbManager::runMigrations()
{
    return DBMigrations::instance().apply();
}

// ============================================================================
// НОВЫЕ МЕТОДЫ ДЛЯ ЧТЕНИЯ КОНФИГУРАЦИИ УСТРОЙСТВ ИЗ БД
// ============================================================================

QList<SensorConfig> DbManager::getActiveSensors()
{
    QList<SensorConfig> sensors;

    // ✅ Читаем из отдельных колонок
    QString query = "SELECT sensor_id, name, protocol, host, port, community, oid, "
                    "modbus_address, modbus_register, polling_interval, is_active "
                    "FROM sensors WHERE is_active = true";

    QSqlQuery sqlQuery = executeQuery(query);
    if (!sqlQuery.isActive()) {
        Logger::instance().error("Failed to load sensors: " + m_lastError);
        return sensors;
    }

    while (sqlQuery.next()) {
        SensorConfig sensor;
        sensor.sensorId = sqlQuery.value("sensor_id").toString();
        sensor.name = sqlQuery.value("name").toString();
        sensor.protocol = sqlQuery.value("protocol").toString();
        sensor.host = sqlQuery.value("host").toString();

        int p = sqlQuery.value("port").toInt();
        sensor.port = (p == 0) ? 161 : p;

        sensor.community = sqlQuery.value("community").toString();
        sensor.oid = sqlQuery.value("oid").toString();

        int ma = sqlQuery.value("modbus_address").toInt();
        sensor.modbusAddress = (ma == 0) ? 1 : ma;

        sensor.modbusRegister = sqlQuery.value("modbus_register").toInt();

        int pi = sqlQuery.value("polling_interval").toInt();
        sensor.pollIntervalMs = (pi == 0) ? 5000 : pi;

        sensor.enabled = sqlQuery.value("is_active").toBool();

        sensors.append(sensor);
    }
    return sensors;
}

QList<DeviceConfig> DbManager::getActiveDeviceConfigs()
{
    QList<SensorConfig> allSensors = getActiveSensors();
    QMap<QString, DeviceConfig> deviceMap;

    for (const SensorConfig &sensor : allSensors) {
        QString key = QString("%1:%2:%3").arg(sensor.protocol).arg(sensor.host).arg(sensor.port);

        if (!deviceMap.contains(key)) {
            DeviceConfig device;
            device.deviceId = key;
            device.host = sensor.host;
            device.port = sensor.port;
            device.protocol = sensor.protocol;
            device.community = sensor.community;
            device.timeoutMs = 2000;
            deviceMap[key] = device;
        }
        deviceMap[key].sensors.append(sensor);
    }
    return deviceMap.values();
}
