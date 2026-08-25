#ifndef SENSOR_H
#define SENSOR_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include "../core_global.h"
#include "sensortype.h"
#include "sensorreading.h"

class CORE_EXPORT Sensor
{
public:
    Sensor();
    explicit Sensor(const QJsonObject &json);

    // Геттеры и сеттеры
    QString sensorId() const { return m_sensorId; }
    void setSensorId(const QString &id) { m_sensorId = id; }

    int typeId() const { return m_typeId; }
    void setTypeId(int id) { m_typeId = id; }

    QString objectId() const { return m_objectId; }
    void setObjectId(const QString &id) { m_objectId = id; }

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString description() const { return m_description; }
    void setDescription(const QString &desc) { m_description = desc; }

    qreal positionX() const { return m_positionX; }
    void setPositionX(qreal x) { m_positionX = x; }

    qreal positionY() const { return m_positionY; }
    void setPositionY(qreal y) { m_positionY = y; }

    QString status() const { return m_status; }
    void setStatus(const QString &status) { m_status = status; }

    qreal lastValue() const { return m_lastValue; }
    void setLastValue(qreal value) { m_lastValue = value; }

    QDateTime lastUpdate() const { return m_lastUpdate; }
    void setLastUpdate(const QDateTime &dt) { m_lastUpdate = dt; }

    QDateTime installDate() const { return m_installDate; }
    void setInstallDate(const QDateTime &dt) { m_installDate = dt; }

    SensorType sensorType() const { return m_sensorType; }
    void setSensorType(const SensorType &type) { m_sensorType = type; }

    bool hasHistory() const { return !m_history.isEmpty(); }
    QList<SensorReading> history() const { return m_history; }
    void setHistory(const QList<SensorReading> &history) { m_history = history; }
    void addReading(const SensorReading &reading) { m_history.append(reading); }

    // Сериализация
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

private:
    QString m_sensorId;
    int m_typeId = 0;
    QString m_objectId;
    QString m_name;
    QString m_description;
    qreal m_positionX = 50.0;
    qreal m_positionY = 50.0;
    QString m_status = "active";
    qreal m_lastValue = 0.0;
    QDateTime m_lastUpdate;
    QDateTime m_installDate;

    SensorType m_sensorType;
    QList<SensorReading> m_history;
};

#endif // SENSOR_H