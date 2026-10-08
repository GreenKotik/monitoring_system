#include "monitoring_service.h"
#include "utils/logger.h"

MonitoringService::MonitoringService(QObject *parent)
    : QObject(parent)
    , m_snmpCollector(new SnmpCollector(this))
    , m_modbusCollector(new ModbusCollector(this))
    , m_tcpCollector(new TcpAsciiCollector(this))
    , m_configRefreshTimer(new QTimer(this))
    , m_isRunning(false)
{
    qInfo() << "MonitoringService created";

    // Подключаем сигналы коллекторов к единым слотам
    connect(m_snmpCollector, &SnmpCollector::dataReceived, this, &MonitoringService::onDataReceived);
    connect(m_snmpCollector, &SnmpCollector::errorOccurred, this, &MonitoringService::onErrorOccurred);

    // Таймер автообновления конфигурации (каждые 30 секунд)
    connect(m_configRefreshTimer, &QTimer::timeout, this, &MonitoringService::onRefreshConfiguration);
}

MonitoringService::~MonitoringService()
{
    stop();
}

bool MonitoringService::initialize()
{
    qInfo() << "Initializing Monitoring Service...";

    if (!DbManager::instance().isConnected()) {
        qCritical() << "Database is not connected!";
        return false;
    }

    qInfo() << "Loading configuration from database...";
    if (!loadConfigurationFromDatabase()) {
        qWarning() << "Failed to load configuration or no devices found";
    }

    setupCollectors();
    qInfo() << "Monitoring Service initialized successfully";
    return true;
}

void MonitoringService::start()
{
    if (m_isRunning) return;

    qInfo() << "Starting Monitoring Service...";

    if (m_snmpCollector->deviceIds().size() > 0) {
        qInfo() << "Starting SNMP Collector...";
        m_snmpCollector->start(5000);
    }

    if (m_modbusCollector->devices().size() > 0) {
        qInfo() << "Starting Modbus Collector...";
        m_modbusCollector->start(5000);
    }

    if (m_tcpCollector->deviceIds().size() > 0) {
        qInfo() << "Starting TCP Collector...";
        m_tcpCollector->start(5000);
    }

    // Запускаем таймер обновления конфигурации (каждые 10/30 секунд)
    m_configRefreshTimer->start(10000);

    m_isRunning = true;
    qInfo() << "Monitoring Service started successfully";
}

void MonitoringService::stop()
{
    if (!m_isRunning) return;

    qInfo() << "Stopping Monitoring Service...";
    m_configRefreshTimer->stop();

    m_snmpCollector->stop();
    m_modbusCollector->stop();
    m_tcpCollector->stop();

    m_isRunning = false;
    qInfo() << "Monitoring Service stopped";
}

void MonitoringService::onRefreshConfiguration()
{
    qInfo() << "Refreshing device configuration from database...";
    loadConfigurationFromDatabase();

    // Перезапускаем коллекторы, если появились новые устройства
    if (m_isRunning) {
        if (m_snmpCollector->deviceIds().size() > 0 && !m_snmpCollector->isRunning()) {
            m_snmpCollector->start(5000);
        }
        if (m_modbusCollector->devices().size() > 0 && !m_modbusCollector->isRunning()) {
            m_modbusCollector->start(5000);
        }
    }
}

bool MonitoringService::loadConfigurationFromDatabase()
{
    QList<DeviceConfig> devices = DbManager::instance().getActiveDeviceConfigs();

    if (devices.isEmpty()) {
        qWarning() << "No active devices found in database";
        return false;
    }

    // Очищаем текущие устройства перед загрузкой новых
    m_snmpCollector->clearDevices();
    m_modbusCollector->clearDevices();
    m_tcpCollector->clearDevices();

    for (const DeviceConfig &device : devices) {
        if (device.protocol == "snmp") {
            SnmpCollector::SnmpDevice snmpDevice;
            snmpDevice.deviceId = device.deviceId;
            snmpDevice.host = device.host;
            snmpDevice.port = device.port;
            snmpDevice.community = device.community;
            snmpDevice.timeoutMs = device.timeoutMs;
            snmpDevice.enabled = true;

            for (const SensorConfig &sensor : device.sensors) {
                if (!sensor.oid.isEmpty()) {
                    snmpDevice.oids[sensor.sensorId] = sensor.oid;
                }
            }

            if (!snmpDevice.oids.isEmpty()) {
                m_snmpCollector->addDevice(snmpDevice);
                qInfo() << "Configured SNMP device:" << snmpDevice.deviceId << "with" << snmpDevice.oids.size() << "OIDs";
            }
        }
        // Здесь можно добавить аналогичную логику для modbus и tcp_ascii
    }

    qInfo() << "Loaded" << devices.size() << "device configurations from database";
    return true;
}

void MonitoringService::setupCollectors()
{
    qInfo() << "Setting up collectors...";
    // Дополнительная инициализация, если нужна
    qInfo() << "Collectors setup completed";
}

void MonitoringService::onDataReceived(const QString &sensorId, qreal value, const QDateTime &timestamp)
{
    Logger::instance().info(QString("Data received: %1 = %2 at %3").arg(sensorId).arg(value).arg(timestamp.toString("hh:mm:ss")));
    DbManager::instance().addReading(sensorId, value);
}

void MonitoringService::onErrorOccurred(const QString &deviceId, const QString &error)
{
    Logger::instance().error(QString("Collector error on %1: %2").arg(deviceId).arg(error));
}
