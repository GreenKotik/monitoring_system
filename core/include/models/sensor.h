#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

class Sensor
{
public:
    Sensor();
    ~Sensor();

    // Геттеры
    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QString typeId() const { return m_typeId; }
    QString typeName() const { return m_typeName; }
    QString objectId() const { return m_objectId; }
    QString objectName() const { return m_objectName; }
    QString unit() const { return m_unit; }
    double minValue() const { return m_minValue; }
    double maxValue() const { return m_maxValue; }
    double lastValue() const { return m_lastValue; }
    QString status() const { return m_status; }
    int pollingInterval() const { return m_pollingInterval; }
    QString connectionParams() const { return m_connectionParams; }
    QDateTime lastUpdate() const { return m_lastUpdate; }
    QDateTime createdAt() const { return m_createdAt; }
    bool isActive() const { return m_isActive; }
    bool isValid() const { return !m_id.isEmpty(); }
    QString description() const { return m_description; }
    double positionX() const { return m_positionX; }
    double positionY() const { return m_positionY; }

    // Сеттеры
    void setId(const QString &id) { m_id = id; }
    void setName(const QString &name) { m_name = name; }
    void setTypeId(const QString &typeId) { m_typeId = typeId; }
    void setTypeName(const QString &typeName) { m_typeName = typeName; }
    void setObjectId(const QString &objectId) { m_objectId = objectId; }
    void setObjectName(const QString &objectName) { m_objectName = objectName; }
    void setUnit(const QString &unit) { m_unit = unit; }
    void setMinValue(double min) { m_minValue = min; }
    void setMaxValue(double max) { m_maxValue = max; }
    void setLastValue(double value) { m_lastValue = value; }
    void setStatus(const QString &status) { m_status = status; }
    void setPollingInterval(int interval) { m_pollingInterval = interval; }
    void setConnectionParams(const QString &params) { m_connectionParams = params; }
    void setLastUpdate(const QDateTime &dt) { m_lastUpdate = dt; }
    void setCreatedAt(const QDateTime &dt) { m_createdAt = dt; }
    void setIsActive(bool active) { m_isActive = active; }
    void setDescription(const QString &desc) { m_description = desc; }
    void setPositionX(double x) { m_positionX = x; }
    void setPositionY(double y) { m_positionY = y; }

    // JSON сериализация
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

    // Операторы сравнения
    bool operator==(const Sensor &other) const;
    bool operator!=(const Sensor &other) const;

private:
    QString m_id;
    QString m_name;
    QString m_typeId;
    QString m_typeName;
    QString m_objectId;
    QString m_objectName;
    QString m_unit;
    double m_minValue = 0.0;
    double m_maxValue = 100.0;
    double m_lastValue = 0.0;
    QString m_status = "normal";
    int m_pollingInterval = 60;
    QString m_connectionParams;
    QDateTime m_lastUpdate;
    QDateTime m_createdAt;
    bool m_isActive = true;
    QString m_description;
    double m_positionX = 0.0;
    double m_positionY = 0.0;
};
