#include "database/dbmanager.h"
#include "database/dbmigrations.h"
#include "utils/logger.h"
#include <QSqlRecord>
#include <QVariant>
#include <QDebug>

DbManager& DbManager::instance()
{
    static DbManager instance;
    return instance;
}

DbManager::~DbManager()
{
    disconnect();
}

bool DbManager::runMigrations()
{
    return DbMigrations::runMigrations();
}

bool DbManager::initialize(const QString &driver, const QString &database,
                           const QString &host, int port,
                           const QString &user, const QString &password)
{
    if (m_db.isOpen()) return true;

    m_db = QSqlDatabase::addDatabase(driver);  // Используем переданный driver
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

    // Миграции — используем правильное имя класса из вашего проекта
    if (!runMigrations()) {
        Logger::instance().error("Database migrations failed");
        return false;
    }
    return true;
}


void DbManager::disconnect()
{
    if (m_db.isOpen()) m_db.close();
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

// ========================================================================
// НОВЫЕ МЕТОДЫ ДЛЯ ЧТЕНИЯ КОНФИГУРАЦИИ УСТРОЙСТВ (Добавить в самый конец файла)
// ========================================================================

QList<SensorConfig> DbManager::getActiveSensors()
{
    QList<SensorConfig> sensors;
    QString query = "SELECT sensor_id, name, protocol, host, port, community, oid, "
                    "modbus_address, modbus_register, poll_interval, enabled, "
                    "command, terminator, regex, checksum_type "
                    "FROM sensors WHERE enabled = true";

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
        sensor.port = sqlQuery.value("port").toInt();
        if (sensor.port == 0) sensor.port = 161;
        sensor.oid = sqlQuery.value("oid").toString();
        int modbusAddr = sqlQuery.value("modbus_address").toInt();
        sensor.modbusAddress = (modbusAddr == 0) ? 1 : modbusAddr;
        int modbusReg = sqlQuery.value("modbus_register").toInt();
        sensor.modbusRegister = modbusReg;
        int pollInt = sqlQuery.value("poll_interval").toInt();
        sensor.pollIntervalMs = (pollInt == 0) ? 5000 : pollInt;
        sensor.enabled = sqlQuery.value("enabled").toBool();
        sensor.command = sqlQuery.value("command").toString();
        sensor.terminator = sqlQuery.value("terminator").toString();
        sensor.regex = sqlQuery.value("regex").toString();
        sensor.checksumType = sqlQuery.value("checksum_type").toString();
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

bool DbManager::addReading(const QString &sensorId, qreal value)
{
    QString query = "INSERT INTO sensor_readings (sensor_id, value) VALUES (?, ?)";
    QVariantList params;
    params.append(sensorId);
    params.append(value);
    return executeQuery(query, params).isActive();
}

// ============= Object CRUD =============

bool DbManager::createObject(const Object &object) {
    QString query = "INSERT INTO objects (id, name, object_type_id, parent_object_id, "
                    "description, position_x, position_y, size_width, size_height, "
                    "color_code, svg_scheme_path, is_active) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    QVariantList params;
    params << object.id()
           << object.name()
           << object.typeId()
           << object.parentObjectId()
           << object.description()
           << object.positionX()
           << object.positionY()
           << object.sizeWidth()
           << object.sizeHeight()
           << object.colorCode()
           << object.svgSchemePath()
           << (object.isActive() ? 1 : 0);

    return executeQuery(query, params).lastError().type() == QSqlError::NoError;
}

// УБИРАЕМ const
QList<Object> DbManager::getObjects(const QString &parentId) {
    QList<Object> objects;

    QString query = "SELECT * FROM objects";
    QVariantList params;

    if (!parentId.isEmpty()) {
        query += " WHERE parent_object_id = ?";
        params << parentId;
    } else {
        query += " WHERE parent_object_id IS NULL OR parent_object_id = ''";
    }

    QSqlQuery sqlQuery = executeQuery(query, params);

    while (sqlQuery.next()) {
        objects.append(parseObject(sqlQuery));
    }

    return objects;
}

// УБИРАЕМ const
Object DbManager::getObjectById(const QString &id) {
    QString query = "SELECT * FROM objects WHERE id = ?";
    QSqlQuery sqlQuery = executeQuery(query, {id});

    if (sqlQuery.next()) {
        return parseObject(sqlQuery);
    }

    return Object();
}

bool DbManager::updateObject(const Object &object) {
    QString query = "UPDATE objects SET name = ?, object_type_id = ?, "
                    "description = ?, position_x = ?, position_y = ?, "
                    "size_width = ?, size_height = ?, color_code = ?, "
                    "svg_scheme_path = ?, is_active = ?, updated_at = CURRENT_TIMESTAMP "
                    "WHERE id = ?";

    QVariantList params;
    params << object.name()
           << object.typeId()
           << object.description()
           << object.positionX()
           << object.positionY()
           << object.sizeWidth()
           << object.sizeHeight()
           << object.colorCode()
           << object.svgSchemePath()
           << (object.isActive() ? 1 : 0)
           << object.id();

    return executeQuery(query, params).lastError().type() == QSqlError::NoError;
}

bool DbManager::deleteObject(const QString &id) {
    QList<Object> children = getObjects(id);
    for (const Object &child : children) {
        deleteObject(child.id());
    }

    QList<Sensor> sensors = getSensors(id);
    for (const Sensor &sensor : sensors) {
        deleteSensor(sensor.id());
    }

    QString query = "DELETE FROM objects WHERE id = ?";
    return executeQuery(query, {id}).lastError().type() == QSqlError::NoError;
}

// ============= Sensor CRUD =============

bool DbManager::createSensor(const Sensor &sensor) {
    QString query = "INSERT INTO sensors (id, name, sensor_type_id, object_id, unit, "
                    "min_value, max_value, last_value, status, polling_interval, "
                    "connection_params, is_active) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    QVariantList params;
    params << sensor.id()
           << sensor.name()
           << sensor.typeId()
           << sensor.objectId()
           << sensor.unit()
           << sensor.minValue()
           << sensor.maxValue()
           << sensor.lastValue()
           << sensor.status()
           << sensor.pollingInterval()
           << sensor.connectionParams()
           << (sensor.isActive() ? 1 : 0);

    return executeQuery(query, params).lastError().type() == QSqlError::NoError;
}

// УБИРАЕМ const
QList<Sensor> DbManager::getSensors(const QString &objectId) {
    QList<Sensor> sensors;

    QString query = "SELECT * FROM sensors";
    QVariantList params;

    if (!objectId.isEmpty()) {
        query += " WHERE object_id = ?";
        params << objectId;
    }

    QSqlQuery sqlQuery = executeQuery(query, params);

    while (sqlQuery.next()) {
        sensors.append(parseSensor(sqlQuery));
    }

    return sensors;
}

// УБИРАЕМ const
Sensor DbManager::getSensorById(const QString &id) {
    QString query = "SELECT * FROM sensors WHERE id = ?";
    QSqlQuery sqlQuery = executeQuery(query, {id});

    if (sqlQuery.next()) {
        return parseSensor(sqlQuery);
    }

    return Sensor();
}

bool DbManager::updateSensor(const Sensor &sensor) {
    QString query = "UPDATE sensors SET name = ?, sensor_type_id = ?, unit = ?, "
                    "min_value = ?, max_value = ?, last_value = ?, status = ?, "
                    "polling_interval = ?, connection_params = ?, is_active = ? "
                    "WHERE id = ?";

    QVariantList params;
    params << sensor.name()
           << sensor.typeId()
           << sensor.unit()
           << sensor.minValue()
           << sensor.maxValue()
           << sensor.lastValue()
           << sensor.status()
           << sensor.pollingInterval()
           << sensor.connectionParams()
           << (sensor.isActive() ? 1 : 0)
           << sensor.id();

    return executeQuery(query, params).lastError().type() == QSqlError::NoError;
}

bool DbManager::deleteSensor(const QString &id) {
    QString deleteReadings = "DELETE FROM sensor_readings WHERE sensor_id = ?";
    executeQuery(deleteReadings, {id});

    QString query = "DELETE FROM sensors WHERE id = ?";
    return executeQuery(query, {id}).lastError().type() == QSqlError::NoError;
}

// ============= Readings =============
/*
bool DBManager::addReading(const QString &sensorId, double value) {
    QString query = "INSERT INTO sensor_readings (id, sensor_id, value) "
                    "VALUES (?, ?, ?)";

    QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QVariantList params;
    params << id << sensorId << value;

    if (executeQuery(query, params).lastError().type() != QSqlError::NoError) {
        return false;
    }

    QString updateSensor = "UPDATE sensors SET last_value = ?, last_update = CURRENT_TIMESTAMP "
                           "WHERE id = ?";
    return executeQuery(updateSensor, {value, sensorId}).lastError().type() == QSqlError::NoError;
}
*/
// УБИРАЕМ const
QList<SensorReading> DbManager::getHistory(const QString &sensorId, int count) {
    QList<SensorReading> readings;

    QString query = "SELECT * FROM sensor_readings "
                    "WHERE sensor_id = ? "
                    "ORDER BY timestamp DESC LIMIT ?";

    QSqlQuery sqlQuery = executeQuery(query, {sensorId, count});

    while (sqlQuery.next()) {
        readings.prepend(parseReading(sqlQuery));
    }

    return readings;
}

// УБИРАЕМ const
QList<SensorReading> DbManager::getHistory(const QString &sensorId,
                                           const QDateTime &from,
                                           const QDateTime &to) {
    QList<SensorReading> readings;

    QString query = "SELECT * FROM sensor_readings "
                    "WHERE sensor_id = ? AND timestamp BETWEEN ? AND ? "
                    "ORDER BY timestamp ASC";

    QVariantList params;
    params << sensorId << from.toString(Qt::ISODate) << to.toString(Qt::ISODate);

    QSqlQuery sqlQuery = executeQuery(query, params);

    while (sqlQuery.next()) {
        readings.append(parseReading(sqlQuery));
    }

    return readings;
}

// ============= Parsers =============

Object DbManager::parseObject(const QSqlQuery &query) const {
    Object obj;
    obj.setId(query.value("id").toString());
    obj.setName(query.value("name").toString());
    obj.setTypeId(query.value("object_type_id").toString());
    obj.setParentObjectId(query.value("parent_object_id").toString());
    obj.setDescription(query.value("description").toString());
    obj.setPositionX(query.value("position_x").toDouble());
    obj.setPositionY(query.value("position_y").toDouble());
    obj.setSizeWidth(query.value("size_width").toDouble());
    obj.setSizeHeight(query.value("size_height").toDouble());
    obj.setColorCode(query.value("color_code").toString());
    obj.setSvgSchemePath(query.value("svg_scheme_path").toString());
    obj.setIsActive(query.value("is_active").toBool());
    obj.setCreatedAt(query.value("created_at").toDateTime());
    obj.setUpdatedAt(query.value("updated_at").toDateTime());
    return obj;
}

Sensor DbManager::parseSensor(const QSqlQuery &query) const {
    Sensor sensor;
    sensor.setId(query.value("id").toString());
    sensor.setName(query.value("name").toString());
    sensor.setTypeId(query.value("sensor_type_id").toString());
    sensor.setObjectId(query.value("object_id").toString());
    sensor.setUnit(query.value("unit").toString());
    sensor.setMinValue(query.value("min_value").toDouble());
    sensor.setMaxValue(query.value("max_value").toDouble());
    sensor.setLastValue(query.value("last_value").toDouble());
    sensor.setStatus(query.value("status").toString());
    sensor.setPollingInterval(query.value("polling_interval").toInt());
    sensor.setConnectionParams(query.value("connection_params").toString());
    sensor.setIsActive(query.value("is_active").toBool());
    sensor.setLastUpdate(query.value("last_update").toDateTime());
    sensor.setCreatedAt(query.value("created_at").toDateTime());
    return sensor;
}

SensorReading DbManager::parseReading(const QSqlQuery &query) const {
    SensorReading reading;
    reading.setId(query.value("id").toString());
    reading.setSensorId(query.value("sensor_id").toString());
    reading.setValue(query.value("value").toDouble());
    reading.setQuality(query.value("quality").toString());
    reading.setTimestamp(query.value("timestamp").toDateTime());
    return reading;
}

