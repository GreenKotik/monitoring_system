#pragma once

#include <QString>
#include <QJsonObject>
#include <QDateTime>

class SensorReading
{
public:
    SensorReading();
    ~SensorReading();

    // Геттеры
    QString id() const { return m_id; }
    QString sensorId() const { return m_sensorId; }
    double value() const { return m_value; }
    QString quality() const { return m_quality; }
    QDateTime timestamp() const { return m_timestamp; }
    bool isValid() const { return !m_id.isEmpty(); }

    // Сеттеры
    void setId(const QString &id) { m_id = id; }
    void setSensorId(const QString &sensorId) { m_sensorId = sensorId; }
    void setValue(double value) { m_value = value; }
    void setQuality(const QString &quality) { m_quality = quality; }
    void setTimestamp(const QDateTime &timestamp) { m_timestamp = timestamp; }

    // JSON сериализация
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

private:
    QString m_id;
    QString m_sensorId;
    double m_value = 0.0;
    QString m_quality = "normal";
    QDateTime m_timestamp;
};