#include "collectors/snmpcollector.h"
#include "collectors/snmpclient.h"
#include "utils/logger.h"  // ✅ Исправлен путь (без "core/")
#include <QDebug>

SnmpCollector::SnmpCollector(QObject *parent)
    : QObject(parent)
    , m_pollTimer(new QTimer(this))
    , m_isRunning(false)
    , m_enabled(true)
{
    connect(m_pollTimer, &QTimer::timeout, this, &SnmpCollector::onPollTimerTimeout);
}

SnmpCollector::~SnmpCollector()
{
    stop();
}

void SnmpCollector::addDevice(const SnmpDevice &device)
{
    if (device.deviceId.isEmpty()) {
        Logger::instance().warning("Cannot add device: deviceId is empty");
        return;
    }

    m_devices[device.deviceId] = device;
    Logger::instance().info(QString("SNMP device added: %1 (%2)")
                            .arg(device.deviceId).arg(device.host));
}

void SnmpCollector::removeDevice(const QString &deviceId)
{
    if (m_devices.remove(deviceId) > 0) {
        Logger::instance().info(QString("SNMP device removed: %1").arg(deviceId));
    }
}

void SnmpCollector::clearDevices()
{
    m_devices.clear();
    Logger::instance().info("All SNMP devices cleared");
}

void SnmpCollector::start(int intervalMs)
{
    if (m_isRunning) {
        Logger::instance().warning("SNMP collector is already running");
        return;
    }

    if (m_devices.isEmpty()) {
        Logger::instance().warning("Cannot start SNMP collector: no devices configured");
        return;
    }

    m_pollTimer->start(intervalMs);
    m_isRunning = true;
    Logger::instance().info(QString("SNMP collector started with interval %1 ms").arg(intervalMs));

    emit started();

    // Немедленный первый опрос
    pollNow();
}

void SnmpCollector::stop()
{
    if (!m_isRunning) return;

    m_pollTimer->stop();
    m_isRunning = false;
    Logger::instance().info("SNMP collector stopped");

    emit stopped();
}

void SnmpCollector::pollNow()
{
    if (m_devices.isEmpty()) {
        Logger::instance().warning("No devices to poll");
        return;
    }

    int enabledCount = 0;
    Logger::instance().info(QString("Starting SNMP poll for %1 devices").arg(m_devices.size()));

    for (auto it = m_devices.begin(); it != m_devices.end(); ++it) {
        const SnmpDevice &device = it.value();

        // Пропускаем отключенные устройства
        if (!device.enabled) {
            continue;
        }

        pollDevice(device);
        enabledCount++;
    }

    emit pollCompleted(enabledCount);
}

bool SnmpCollector::isEnabled() const
{
    return m_enabled;
}

void SnmpCollector::setEnabled(bool enabled)
{
    if (m_enabled == enabled) return;

    m_enabled = enabled;
    Logger::instance().info(QString("SNMP collector %1").arg(enabled ? "enabled" : "disabled"));

    if (!enabled && m_isRunning) {
        stop();
    }
}

QList<SnmpCollector::SnmpDevice> SnmpCollector::devices() const
{
    return m_devices.values();
}

QList<QString> SnmpCollector::deviceIds() const
{
    return m_devices.keys();
}

bool SnmpCollector::isRunning() const
{
    return m_isRunning;
}

void SnmpCollector::onPollTimerTimeout()
{
    pollNow();
}

void SnmpCollector::pollDevice(const SnmpDevice &device)
{
    if (!device.enabled) {
        return;
    }

    Logger::instance().debug(QString("Polling SNMP device: %1 (%2)")
                             .arg(device.deviceId).arg(device.host));

    SnmpClient client;
    QDateTime timestamp = QDateTime::currentDateTime();

    for (auto it = device.oids.begin(); it != device.oids.end(); ++it) {
        const QString &sensorId = it.key();
        const QString &oid = it.value();

        // ✅ Используем QVariant вместо QVariantMap
        QVariant result;

        bool success = client.get(device.host, device.port, device.community,
                                   oid, result, device.timeoutMs);

        if (success) {
            if (result.isValid()) {
                // ✅ Проверяем, является ли результат map'ом или скалярным значением
                qreal value = 0.0;
                bool ok = false;

                if (result.type() == QVariant::Map) {
                    // Если это map, извлекаем значение по ключу "value"
                    QVariantMap resultMap = result.toMap();
                    value = resultMap.value("value").toDouble(&ok);
                } else {
                    // Если это скалярное значение, преобразуем напрямую
                    value = result.toDouble(&ok);
                }

                if (ok) {
                    Logger::instance().debug(QString("SNMP data received: %1 = %2")
                                             .arg(sensorId).arg(value));

                    emit dataReceived(sensorId, value, timestamp);
                } else {
                    QString strValue = result.toString();
                    Logger::instance().warning(QString("SNMP value is not numeric for %1: %2")
                                               .arg(sensorId).arg(strValue));
                }
            } else {
                Logger::instance().warning(QString("No value in SNMP response for %1").arg(sensorId));
            }
        } else {
            QString error = client.lastError();
            Logger::instance().error(QString("SNMP poll failed for device %1, sensor %2: %3")
                                     .arg(device.deviceId).arg(sensorId).arg(error));
            emit errorOccurred(device.deviceId, error);
        }
    }
}
