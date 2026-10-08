#include "models/sensor.h"
#include <QDebug>

Sensor::Sensor()
    : m_minValue(0.0)
    , m_maxValue(100.0)
    , m_lastValue(0.0)
    , m_status("normal")
    , m_pollingInterval(60)
    , m_isActive(true)
{
}

Sensor::~Sensor() {}

QJsonObject Sensor::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["sensor_type_id"] = m_typeId;
    json["sensor_type_name"] = m_typeName;
    json["object_id"] = m_objectId;
    json["object_name"] = m_objectName;
    json["unit"] = m_unit;
    json["min_value"] = m_minValue;
    json["max_value"] = m_maxValue;
    json["last_value"] = m_lastValue;
    json["status"] = m_status;
    json["polling_interval"] = m_pollingInterval;
    json["connection_params"] = m_connectionParams;
    json["is_active"] = m_isActive;
    
    if (m_lastUpdate.isValid()) {
        json["last_update"] = m_lastUpdate.toString(Qt::ISODate);
    }
    if (m_createdAt.isValid()) {
        json["created_at"] = m_createdAt.toString(Qt::ISODate);
    }
    
    return json;
}

void Sensor::fromJson(const QJsonObject &json)
{
    m_id = json["id"].toString();
    m_name = json["name"].toString();
    m_typeId = json["sensor_type_id"].toString();
    m_typeName = json["sensor_type_name"].toString();
    m_objectId = json["object_id"].toString();
    m_objectName = json["object_name"].toString();
    m_unit = json["unit"].toString();
    m_minValue = json["min_value"].toDouble(0.0);
    m_maxValue = json["max_value"].toDouble(100.0);
    m_lastValue = json["last_value"].toDouble(0.0);
    m_status = json["status"].toString("normal");
    m_pollingInterval = json["polling_interval"].toInt(60);
    m_connectionParams = json["connection_params"].toString();
    m_isActive = json["is_active"].toBool(true);
    
    QString lastUpdateStr = json["last_update"].toString();
    if (!lastUpdateStr.isEmpty()) {
        m_lastUpdate = QDateTime::fromString(lastUpdateStr, Qt::ISODate);
    }
    
    QString createdAtStr = json["created_at"].toString();
    if (!createdAtStr.isEmpty()) {
        m_createdAt = QDateTime::fromString(createdAtStr, Qt::ISODate);
    }
}

bool Sensor::operator==(const Sensor &other) const
{
    return m_id == other.m_id;
}

bool Sensor::operator!=(const Sensor &other) const
{
    return !(*this == other);
}