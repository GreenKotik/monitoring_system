#include "collectors/snmpcollector.h"
#include <QDebug>
#include <QProcess>
#include <QRegularExpression>

SnmpCollector::SnmpCollector(QObject *parent)
    : BaseCollector("SNMP Collector", parent)
{
}

void SnmpCollector::addDevice(const SnmpDevice &device)
{
    if (device.deviceId.isEmpty()) return;

    // Проверяем, нет ли уже такого устройства
    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i].deviceId == device.deviceId) {
            m_devices[i] = device;
            qDebug() << "Device" << device.deviceId << "updated";
            return;
        }
    }

    m_devices.append(device);
    qDebug() << "Device" << device.deviceId << "added";
}

void SnmpCollector::removeDevice(const QString &deviceId)
{
    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i].deviceId == deviceId) {
            m_devices.removeAt(i);
            qDebug() << "Device" << deviceId << "removed";
            return;
        }
    }
}

void SnmpCollector::poll()
{
    if (m_devices.isEmpty()) {
        qDebug() << "No SNMP devices configured";
        return;
    }

    for (const SnmpDevice &device : m_devices) {
        for (auto it = device.oids.begin(); it != device.oids.end(); ++it) {
            const QString &sensorId = it.key();
            const QString &oid = it.value();

            qreal value = getValue(device.host, device.port, oid, device.community);

            if (value != -1) {
                emit dataReceived(sensorId, value);
                qDebug() << "SNMP: sensor" << sensorId << "=" << value;
            } else {
                emit errorOccurred(sensorId, "Failed to read OID " + oid);
            }
        }
    }
}

qreal SnmpCollector::getValue(const QString &host, int port, const QString &oid, const QString &community)
{
    // Используем snmpget утилиту
    QString command = QString("snmpget -v2c -c %1 %2:%3 %4")
                      .arg(community)
                      .arg(host)
                      .arg(port)
                      .arg(oid);

    QProcess process;
    process.start(command);
    if (!process.waitForFinished(5000)) {
        qDebug() << "SNMP timeout for" << host << oid;
        return -1;
    }

    QString output = process.readAllStandardOutput();
    if (output.isEmpty()) {
        qDebug() << "SNMP no output for" << host << oid;
        return -1;
    }

    // Парсим значение: "iso.3.6.1.2.1.1.1.0 = STRING: value"
    QRegularExpression regex("= (STRING|INTEGER|GAUGE|COUNTER|TIMETICKS): (.+)");
    QRegularExpressionMatch match = regex.match(output);
    if (match.hasMatch()) {
        QString valueStr = match.captured(2).trimmed();
        bool ok;
        qreal value = valueStr.toDouble(&ok);
        return ok ? value : -1;
    }

    return -1;
}