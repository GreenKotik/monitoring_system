#include "models/sensorreading.h"

SensorReading::SensorReading() {}

SensorReading::~SensorReading() {}

QJsonObject SensorReading::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["sensor_id"] = m_sensorId;
    json["value"] = m_value;
    json["quality"] = m_quality;
    
    if (m_timestamp.isValid()) {
        json["timestamp"] = m_timestamp.toString(Qt::ISODate);
    }
    
    return json;
}

void SensorReading::fromJson(const QJsonObject &json)
{
    m_id = json["id"].toString();
    m_sensorId = json["sensor_id"].toString();
    m_value = json["value"].toDouble(0.0);
    m_quality = json["quality"].toString("normal");
    
    QString timestampStr = json["timestamp"].toString();
    if (!timestampStr.isEmpty()) {
        m_timestamp = QDateTime::fromString(timestampStr, Qt::ISODate);
    }
}