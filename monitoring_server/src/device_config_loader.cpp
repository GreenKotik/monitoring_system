#include "device_config_loader.h"
#include "utils/logger.h"

DeviceConfigLoader::DeviceConfigLoader(
    SnmpCollector *snmpCollector,
    ModbusCollector *modbusCollector,
    TcpAsciiCollector *tcpCollector,
    QObject *parent
)
    : QObject(parent)
    , m_snmpCollector(snmpCollector)
    , m_modbusCollector(modbusCollector)
    , m_tcpCollector(tcpCollector)
    , m_refreshTimer(new QTimer(this))
    , m_autoRefreshEnabled(false)
{
    connect(m_refreshTimer, &QTimer::timeout, this, &DeviceConfigLoader::onRefreshTimer);
}

void DeviceConfigLoader::loadConfiguration()
{
    Logger::instance().info("Loading device configuration from database...");

    QList<DbManager::DeviceConfig> devices = DbManager::instance().getActiveDeviceConfigs();

    if (devices.isEmpty()) {
        Logger::instance().warning("No active devices found in database");
        emit configurationLoaded(0);
        return;
    }

    // Очищаем текущие устройства
    m_snmpCollector->clearDevices();
    m_modbusCollector->clearDevices();
    m_tcpCollector->clearDevices();

    // Настраиваем коллекторы
    configureSnmpDevices(devices);
    configureModbusDevices(devices);
    configureTcpDevices(devices);

    Logger::instance().info(QString("Configuration loaded: %1 devices").arg(devices.size()));
    emit configurationLoaded(devices.size());
}

void DeviceConfigLoader::enableAutoRefresh(bool enable, int intervalMs)
{
    m_autoRefreshEnabled = enable;

    if (enable) {
        m_refreshTimer->start(intervalMs);
        Logger::instance().info(QString("Auto-refresh enabled (every %1 ms)").arg(intervalMs));
    } else {
        m_refreshTimer->stop();
        Logger::instance().info("Auto-refresh disabled");
    }
}

void DeviceConfigLoader::onRefreshTimer()
{
    loadConfiguration();
}

void DeviceConfigLoader::configureSnmpDevices(const QList<DbManager::DeviceConfig> &devices)
{
    for (const DbManager::DeviceConfig &device : devices) {
        if (device.protocol != "snmp") continue;

        SnmpCollector::SnmpDevice snmpDevice;
        snmpDevice.deviceId = device.deviceId;
        snmpDevice.host = device.host;
        snmpDevice.port = device.port;
        snmpDevice.community = device.community;
        snmpDevice.timeoutMs = device.timeoutMs;
        snmpDevice.enabled = true;

        // Добавляем все OID для этого устройства
        for (const DbManager::SensorConfig &sensor : device.sensors) {
            if (!sensor.oid.isEmpty()) {
                snmpDevice.oids[sensor.sensorId] = sensor.oid;
            }
        }

        if (!snmpDevice.oids.isEmpty()) {
            m_snmpCollector->addDevice(snmpDevice);
            Logger::instance().info(QString("SNMP device configured: %1 (%2) with %3 OIDs")
                                    .arg(snmpDevice.deviceId)
                                    .arg(snmpDevice.host)
                                    .arg(snmpDevice.oids.size()));
        }
    }
}

void DeviceConfigLoader::configureModbusDevices(const QList<DbManager::DeviceConfig> &devices)
{
    for (const DbManager::DeviceConfig &device : devices) {
        if (device.protocol != "modbus_tcp" && device.protocol != "modbus_rtu") continue;

        // Аналогично для Modbus
        // ModbusCollector::ModbusDevice modbusDevice;
        // ... настройка
        // m_modbusCollector->addDevice(modbusDevice);
    }
}

void DeviceConfigLoader::configureTcpDevices(const QList<DbManager::DeviceConfig> &devices)
{
    for (const DbManager::DeviceConfig &device : devices) {
        if (device.protocol != "tcp_ascii") continue;

        // Аналогично для TCP
        // TcpAsciiCollector::TcpDevice tcpDevice;
        // ... настройка
        // m_tcpCollector->addDevice(tcpDevice);
    }
}
