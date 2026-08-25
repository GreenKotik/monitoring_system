#include "models/sensorreading.h"

SensorReading::SensorReading()
{
}

SensorReading::SensorReading(const QJsonObject &json)
{
    fromJson(json);
}

QJsonObject SensorReading::toJson() const
{
    QJsonObject json;
    json["reading_id"] = static_cast<qint64>(m_readingId);
    json["sensor_id"] = m_sensorId;
    json["value"] = m_value;
    json["timestamp"] = m_timestamp.toString(Qt::ISODate);
    return json;
}

void SensorReading::fromJson(const QJsonObject &json)
{
    m_readingId = json["reading_id"].toVariant().toLongLong();
    m_sensorId = json["sensor_id"].toString();
    m_value = json["value"].toDouble();

    QString timestamp = json["timestamp"].toString();
    if (!timestamp.isEmpty()) {
        m_timestamp = QDateTime::fromString(timestamp, Qt::ISODate);
    }
}