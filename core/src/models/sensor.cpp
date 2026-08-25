#include "models/sensor.h"

Sensor::Sensor()
{
}

Sensor::Sensor(const QJsonObject &json)
{
    fromJson(json);
}

QJsonObject Sensor::toJson() const
{
    QJsonObject json;
    json["sensor_id"] = m_sensorId;
    json["type_id"] = m_typeId;
    json["object_id"] = m_objectId;
    json["name"] = m_name;
    json["description"] = m_description;
    json["position_x"] = m_positionX;
    json["position_y"] = m_positionY;
    json["status"] = m_status;
    json["last_value"] = m_lastValue;
    json["last_update"] = m_lastUpdate.toString(Qt::ISODate);
    json["install_date"] = m_installDate.toString(Qt::ISODate);

    json["sensor_type"] = m_sensorType.toJson();

    QJsonArray historyArray;
    for (const SensorReading &reading : m_history) {
        historyArray.append(reading.toJson());
    }
    json["history"] = historyArray;

    return json;
}

void Sensor::fromJson(const QJsonObject &json)
{
    m_sensorId = json["sensor_id"].toString();
    m_typeId = json["type_id"].toInt();
    m_objectId = json["object_id"].toString();
    m_name = json["name"].toString();
    m_description = json["description"].toString();
    m_positionX = json["position_x"].toDouble();
    m_positionY = json["position_y"].toDouble();
    m_status = json["status"].toString();
    m_lastValue = json["last_value"].toDouble();

    QString lastUpdate = json["last_update"].toString();
    if (!lastUpdate.isEmpty()) {
        m_lastUpdate = QDateTime::fromString(lastUpdate, Qt::ISODate);
    }

    QString installDate = json["install_date"].toString();
    if (!installDate.isEmpty()) {
        m_installDate = QDateTime::fromString(installDate, Qt::ISODate);
    }

    if (json.contains("sensor_type") && json["sensor_type"].isObject()) {
        m_sensorType.fromJson(json["sensor_type"].toObject());
    }

    if (json.contains("history") && json["history"].isArray()) {
        m_history.clear();
        QJsonArray historyArray = json["history"].toArray();
        for (const QJsonValue &value : historyArray) {
            SensorReading reading;
            reading.fromJson(value.toObject());
            m_history.append(reading);
        }
    }
}