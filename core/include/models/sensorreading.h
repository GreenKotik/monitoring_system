#ifndef SENSORREADING_H
#define SENSORREADING_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include "../core_global.h"

class CORE_EXPORT SensorReading
{
public:
    SensorReading();
    explicit SensorReading(const QJsonObject &json);

    qint64 readingId() const { return m_readingId; }
    void setReadingId(qint64 id) { m_readingId = id; }

    QString sensorId() const { return m_sensorId; }
    void setSensorId(const QString &id) { m_sensorId = id; }

    qreal value() const { return m_value; }
    void setValue(qreal value) { m_value = value; }

    QDateTime timestamp() const { return m_timestamp; }
    void setTimestamp(const QDateTime &dt) { m_timestamp = dt; }

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

private:
    qint64 m_readingId = 0;
    QString m_sensorId;
    qreal m_value = 0.0;
    QDateTime m_timestamp = QDateTime::currentDateTime();
};

#endif // SENSORREADING_H