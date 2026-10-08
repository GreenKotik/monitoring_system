#ifndef BASECOLLECTOR_H
#define BASECOLLECTOR_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>
#include <QDebug>
#include <QDateTime>

class BaseCollector : public QObject
{
    Q_OBJECT

public:
    explicit BaseCollector(const QString &name, QObject *parent = nullptr);

    QString name() const { return m_name; }
    bool isRunning() const { return m_isRunning; }
    int interval() const { return m_interval; }
    qint64 lastPollTime() const { return m_lastPollTime; }

    virtual void start(int intervalMs = 5000);
    virtual void stop();
    virtual void poll() = 0;
    virtual void setEnabled(bool enabled) { m_enabled = enabled; }
    virtual bool isEnabled() const { return m_enabled; }

signals:
    void dataReceived(const QString &sensorId, qreal value, const QDateTime &timestamp = QDateTime::currentDateTime());
    void errorOccurred(const QString &sensorId, const QString &error);
    void statusChanged(const QString &status);
    void started();
    void stopped();
    void pollCompleted(int totalReadings);

protected:
    QString m_name;
    QTimer *m_timer;
    bool m_isRunning = false;
    bool m_enabled = true;
    int m_interval = 5000;
    qint64 m_lastPollTime = 0;
    int m_totalReadings = 0;
};

#endif // BASECOLLECTOR_H