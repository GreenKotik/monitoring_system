#ifndef DEVICE_CONFIG_LOADER_H
#define DEVICE_CONFIG_LOADER_H

#include <QObject>
#include <QTimer>
#include "database/dbmanager.h"
#include "collectors/snmpcollector.h"
#include "collectors/modbuscollector.h"
#include "collectors/tcpasciicollector.h"

class DeviceConfigLoader : public QObject
{
    Q_OBJECT

public:
    explicit DeviceConfigLoader(
        SnmpCollector *snmpCollector,
        ModbusCollector *modbusCollector,
        TcpAsciiCollector *tcpCollector,
        QObject *parent = nullptr
    );

    // Загрузить конфигурацию из БД
    void loadConfiguration();

    // Включить/выключить периодическое обновление
    void enableAutoRefresh(bool enable, int intervalMs = 30000);

signals:
    void configurationLoaded(int deviceCount);
    void configurationError(const QString &error);

private slots:
    void onRefreshTimer();

private:
    void configureSnmpDevices(const QList<DbManager::DeviceConfig> &devices);
    void configureModbusDevices(const QList<DbManager::DeviceConfig> &devices);
    void configureTcpDevices(const QList<DbManager::DeviceConfig> &devices);

    SnmpCollector *m_snmpCollector;
    ModbusCollector *m_modbusCollector;
    TcpAsciiCollector *m_tcpCollector;
    QTimer *m_refreshTimer;
    bool m_autoRefreshEnabled;
};

#endif // DEVICE_CONFIG_LOADER_H
