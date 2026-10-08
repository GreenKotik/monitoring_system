#ifndef SNMPCOLLECTOR_H
#define SNMPCOLLECTOR_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QList>

class SnmpClient;

class SnmpCollector : public QObject
{
    Q_OBJECT

public:
    struct SnmpDevice {
        QString deviceId;
        QString host;
        int port = 161;
        QString community;
        int version = 2;              // SNMP version (1 or 2)
        QMap<QString, QString> oids;
        int timeoutMs = 2000;
        bool enabled = true;          // Флаг включен/выключен
    };

    explicit SnmpCollector(QObject *parent = nullptr);
    ~SnmpCollector();

    void addDevice(const SnmpDevice &device);
    void removeDevice(const QString &deviceId);
    void clearDevices();

    void start(int intervalMs);
    void stop();
    void pollNow();

    // Новые методы для совместимости с MonitoringService
    bool isEnabled() const;
    void setEnabled(bool enabled);

    QList<SnmpDevice> devices() const;
    QList<QString> deviceIds() const;
    bool isRunning() const;

signals:
    // Сигнал с timestamp (для совместимости с MonitoringService)
    void dataReceived(const QString &sensorId, qreal value, const QDateTime &timestamp);

    void errorOccurred(const QString &deviceId, const QString &error);

    // Сигналы состояния
    void started();
    void stopped();

    // Сигнал завершения опроса с количеством устройств
    void pollCompleted(int deviceCount);

private slots:
    void onPollTimerTimeout();

private:
    void pollDevice(const SnmpDevice &device);

    QMap<QString, SnmpDevice> m_devices;
    QTimer *m_pollTimer;
    bool m_isRunning;
    bool m_enabled;
};

#endif // SNMPCOLLECTOR_H
