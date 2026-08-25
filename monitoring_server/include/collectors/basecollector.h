#ifndef BASECOLLECTOR_H
#define BASECOLLECTOR_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>
#include <QDebug>

class BaseCollector : public QObject
{
    Q_OBJECT

public:
    explicit BaseCollector(const QString &name, QObject *parent = nullptr);

    QString name() const { return m_name; }
    bool isRunning() const { return m_isRunning; }

    virtual void start(int intervalMs = 5000);
    virtual void stop();
    virtual void poll() = 0;

signals:
    void dataReceived(const QString &sensorId, qreal value);
    void errorOccurred(const QString &sensorId, const QString &error);
    void statusChanged(const QString &status);
    void started();
    void stopped();

protected:
    QString m_name;
    QTimer *m_timer;
    bool m_isRunning = false;
    int m_interval = 5000;
};

#endif // BASECOLLECTOR_H