#ifndef MONITORING_SERVICE_H
#define MONITORING_SERVICE_H

#include <QObject>
#include <QTimer>
#include "collectors/snmpcollector.h"
#include "collectors/modbuscollector.h"
#include "collectors/tcpasciicollector.h"
#include "database/dbmanager.h"

class MonitoringService : public QObject
{
    Q_OBJECT

public:
    explicit MonitoringService(QObject *parent = nullptr);
    ~MonitoringService();

    bool initialize();
    void start();
    void stop();

private slots:
    void onRefreshConfiguration();
    void onDataReceived(const QString &sensorId, qreal value, const QDateTime &timestamp);
    void onErrorOccurred(const QString &deviceId, const QString &error);

private:
    bool loadConfigurationFromDatabase();
    void setupCollectors();

    SnmpCollector *m_snmpCollector;
    ModbusCollector *m_modbusCollector;
    TcpAsciiCollector *m_tcpCollector;

    QTimer *m_configRefreshTimer;
    bool m_isRunning;
};

#endif // MONITORING_SERVICE_H
