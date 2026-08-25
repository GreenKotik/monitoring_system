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

    // Выполняем миграции
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
    QString query = "SELECT o.*, ot.* FROM objects o "
                    "LEFT JOIN object_types ot ON o.object_type_id = ot.type_id ";
    QVariantList params;

    if (!parentId.isEmpty()) {
        query += "WHERE o.parent_object_id = ?";
        params.append(parentId);
    }

    query += " ORDER BY o.name";

    QSqlQuery sqlQuery = executeQuery(query, params);
    if (!sqlQuery.isActive()) return objects;

    while (sqlQuery.next()) {
        QJsonObject json;
        json["object_id"] = sqlQuery.value("object_id").toString();
        json["object_type_id"] = sqlQuery.value("object_type_id").toInt();
        json["parent_object_id"] = sqlQuery.value("parent_object_id").toString();
        json["name"] = sqlQuery.value("name").toString();
        json["description"] = sqlQuery.value("description").toString();
        json["position_x"] = sqlQuery.value("position_x").toDouble();
        json["position_y"] = sqlQuery.value("position_y").toDouble();
        json["size_width"] = sqlQuery.value("size_width").toDouble();
        json["size_height"] = sqlQuery.value("size_height").toDouble();
        json["svg_scheme_path"] = sqlQuery.value("svg_scheme_path").toString();
        json["status"] = sqlQuery.value("status").toString();
        json["created_at"] = sqlQuery.value("created_at").toString();

        QJsonObject typeJson;
        typeJson["type_id"] = sqlQuery.value("type_id").toInt();
        typeJson["type_code"] = sqlQuery.value("type_code").toString();
        typeJson["type_name"] = sqlQuery.value("type_name").toString();
        typeJson["can_have_children"] = sqlQuery.value("can_have_children").toBool();
        typeJson["icon_name"] = sqlQuery.value("icon_name").toString();
        typeJson["color_code"] = sqlQuery.value("color_code").toString();
        typeJson["svg_icon_path"] = sqlQuery.value("svg_icon_path").toString();
        json["object_type"] = typeJson;

        Object obj;
        obj.fromJson(json);
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
        if (obj.objectId() == objectId) {
            return obj;
        }
    }
    return Object();
}

bool DbManager::createObject(const Object &object)
{
    QString query = "INSERT INTO objects (object_id, object_type_id, parent_object_id, "
                    "name, description, position_x, position_y, size_width, size_height, "
                    "svg_scheme_path, status) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    QVariantList params;
    params.append(object.objectId());
    params.append(object.objectTypeId());
    params.append(object.parentObjectId());
    params.append(object.name());
    params.append(object.description());
    params.append(object.positionX());
    params.append(object.positionY());
    params.append(object.sizeWidth());
    params.append(object.sizeHeight());
    params.append(object.svgSchemePath());
    params.append(object.status());

    QSqlQuery sqlQuery = executeQuery(query, params);
    return sqlQuery.isActive();
}

bool DbManager::updateObject(const Object &object)
{
    QString query = "UPDATE objects SET object_type_id = ?, parent_object_id = ?, "
                    "name = ?, description = ?, position_x = ?, position_y = ?, "
                    "size_width = ?, size_height = ?, svg_scheme_path = ?, status = ? "
                    "WHERE object_id = ?";

    QVariantList params;
    params.append(object.objectTypeId());
    params.append(object.parentObjectId());
    params.append(object.name());
    params.append(object.description());
    params.append(object.positionX());
    params.append(object.positionY());
    params.append(object.sizeWidth());
    params.append(object.sizeHeight());
    params.append(object.svgSchemePath());
    params.append(object.status());
    params.append(object.objectId());

    QSqlQuery sqlQuery = executeQuery(query, params);
    return sqlQuery.isActive() && sqlQuery.numRowsAffected() > 0;
}

bool DbManager::deleteObject(const QString &objectId)
{
    QString query = "DELETE FROM objects WHERE object_id = ?";
    QVariantList params;
    params.append(objectId);

    QSqlQuery sqlQuery = executeQuery(query, params);
    return sqlQuery.isActive() && sqlQuery.numRowsAffected() > 0;
}

QList<Sensor> DbManager::getSensors(const QString &objectId)
{
    QList<Sensor> sensors;
    QString query = "SELECT s.*, st.* FROM sensors s "
                    "LEFT JOIN sensor_types st ON s.type_id = st.type_id ";
    QVariantList params;

    if (!objectId.isEmpty()) {
        query += "WHERE s.object_id = ?";
        params.append(objectId);
    }

    query += " ORDER BY s.name";

    QSqlQuery sqlQuery = executeQuery(query, params);
    if (!sqlQuery.isActive()) return sensors;

    while (sqlQuery.next()) {
        QJsonObject json;
        json["sensor_id"] = sqlQuery.value("sensor_id").toString();
        json["type_id"] = sqlQuery.value("type_id").toInt();
        json["object_id"] = sqlQuery.value("object_id").toString();
        json["name"] = sqlQuery.value("name").toString();
        json["description"] = sqlQuery.value("description").toString();
        json["position_x"] = sqlQuery.value("position_x").toDouble();
        json["position_y"] = sqlQuery.value("position_y").toDouble();
        json["status"] = sqlQuery.value("status").toString();
        json["last_value"] = sqlQuery.value("last_value").toDouble();
        json["last_update"] = sqlQuery.value("last_update").toString();
        json["install_date"] = sqlQuery.value("install_date").toString();

        QJsonObject typeJson;
        typeJson["type_id"] = sqlQuery.value("type_id").toInt();
        typeJson["type_code"] = sqlQuery.value("type_code").toString();
        typeJson["type_name"] = sqlQuery.value("type_name").toString();
        typeJson["unit"] = sqlQuery.value("unit").toString();
        typeJson["color_code"] = sqlQuery.value("color_code").toString();
        typeJson["icon_name"] = sqlQuery.value("icon_name").toString();
        typeJson["svg_image_path"] = sqlQuery.value("svg_image_path").toString();
        json["sensor_type"] = typeJson;

        Sensor sensor;
        sensor.fromJson(json);
        sensors.append(sensor);
    }

    return sensors;
}

Sensor DbManager::getSensor(const QString &sensorId)
{
    QList<Sensor> sensors = getSensors();
    for (const Sensor &sensor : sensors) {
        if (sensor.sensorId() == sensorId) {
            return sensor;
        }
    }
    return Sensor();
}

bool DbManager::createSensor(const Sensor &sensor)
{
    QString query = "INSERT INTO sensors (sensor_id, type_id, object_id, name, "
                    "description, position_x, position_y, status, install_date) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";

    QVariantList params;
    params.append(sensor.sensorId());
    params.append(sensor.typeId());
    params.append(sensor.objectId());
    params.append(sensor.name());
    params.append(sensor.description());
    params.append(sensor.positionX());
    params.append(sensor.positionY());
    params.append(sensor.status());
    params.append(sensor.installDate());

    QSqlQuery sqlQuery = executeQuery(query, params);
    return sqlQuery.isActive();
}

bool DbManager::updateSensor(const Sensor &sensor)
{
    QString query = "UPDATE sensors SET type_id = ?, object_id = ?, name = ?, "
                    "description = ?, position_x = ?, position_y = ?, status = ?, "
                    "last_value = ?, last_update = ? "
                    "WHERE sensor_id = ?";

    QVariantList params;
    params.append(sensor.typeId());
    params.append(sensor.objectId());
    params.append(sensor.name());
    params.append(sensor.description());
    params.append(sensor.positionX());
    params.append(sensor.positionY());
    params.append(sensor.status());
    params.append(sensor.lastValue());
    params.append(sensor.lastUpdate());
    params.append(sensor.sensorId());

    QSqlQuery sqlQuery = executeQuery(query, params);
    return sqlQuery.isActive() && sqlQuery.numRowsAffected() > 0;
}

bool DbManager::deleteSensor(const QString &sensorId)
{
    QString query = "DELETE FROM sensors WHERE sensor_id = ?";
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
        reading.setReadingId(sqlQuery.value("reading_id").toLongLong());
        reading.setSensorId(sqlQuery.value("sensor_id").toString());
        reading.setValue(sqlQuery.value("value").toDouble());
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
        reading.setReadingId(sqlQuery.value("reading_id").toLongLong());
        reading.setSensorId(sqlQuery.value("sensor_id").toString());
        reading.setValue(sqlQuery.value("value").toDouble());
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
    return DbMigrations::runMigrations();
}