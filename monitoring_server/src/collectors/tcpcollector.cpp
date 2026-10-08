#include "collectors/tcpcollector.h"
#include <QDebug>
#include <QHostAddress>
#include <QThread>

TcpCollector::TcpCollector(QObject *parent)
    : BaseCollector("TCP/IP Collector", parent)
{
    qDebug() << "TCP/IP Collector initialized";
}

TcpCollector::~TcpCollector()
{
    stop();
    for (auto &conn : m_connections) {
        if (conn.socket) {
            conn.socket->disconnectFromHost();
            delete conn.socket;
        }
    }
    m_connections.clear();
}

void TcpCollector::addDevice(const TcpDevice &device)
{
    if (device.deviceId.isEmpty()) {
        qWarning() << "Cannot add device with empty ID";
        return;
    }

    // Проверяем, нет ли уже такого устройства
    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i].deviceId == device.deviceId) {
            qDebug() << "Device" << device.deviceId << "already exists, updating";
            m_devices[i] = device;
            emit deviceAdded(device.deviceId);
            return;
        }
    }

    m_devices.append(device);
    qDebug() << "Device" << device.deviceId << "added to TCP/IP collector";
    emit deviceAdded(device.deviceId);
}

void TcpCollector::removeDevice(const QString &deviceId)
{
    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i].deviceId == deviceId) {
            disconnectDevice(deviceId);
            m_devices.removeAt(i);
            qDebug() << "Device" << deviceId << "removed from TCP/IP collector";
            emit deviceRemoved(deviceId);
            return;
        }
    }
    qWarning() << "Device" << deviceId << "not found";
}

bool TcpCollector::hasDevice(const QString &deviceId) const
{
    for (const TcpDevice &dev : m_devices) {
        if (dev.deviceId == deviceId) {
            return true;
        }
    }
    return false;
}

void TcpCollector::clearDevices()
{
    for (auto &conn : m_connections) {
        if (conn.socket) {
            conn.socket->disconnectFromHost();
            delete conn.socket;
        }
    }
    m_connections.clear();
    m_devices.clear();
    qDebug() << "All TCP/IP devices cleared";
}

void TcpCollector::setEnabled(bool enabled)
{
    BaseCollector::setEnabled(enabled);
    if (!enabled) {
        stop();
        for (auto &conn : m_connections) {
            if (conn.socket) {
                conn.socket->disconnectFromHost();
            }
        }
    }
}

bool TcpCollector::connectDevice(const QString &deviceId)
{
    if (!hasDevice(deviceId)) {
        qWarning() << "Device" << deviceId << "not found";
        return false;
    }

    if (m_connections.contains(deviceId) && m_connections[deviceId].connected) {
        qDebug() << "Device" << deviceId << "already connected";
        return true;
    }

    // Находим устройство
    TcpDevice device;
    for (const auto &dev : m_devices) {
        if (dev.deviceId == deviceId) {
            device = dev;
            break;
        }
    }

    if (device.deviceId.isEmpty()) {
        return false;
    }

    // Создаём соединение
    TcpConnection conn;
    conn.device = device;
    conn.socket = new QTcpSocket(this);
    conn.connected = false;

    connect(conn.socket, &QTcpSocket::connected, this, &TcpCollector::onConnected);
    connect(conn.socket, &QTcpSocket::disconnected, this, &TcpCollector::onDisconnected);
    connect(conn.socket, &QTcpSocket::readyRead, this, &TcpCollector::onReadyRead);
    connect(conn.socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &TcpCollector::onError);

    m_connections[deviceId] = conn;

    qDebug() << "Connecting to TCP device" << device.host << ":" << device.port;
    conn.socket->connectToHost(device.host, device.port);

    return true;
}

void TcpCollector::disconnectDevice(const QString &deviceId)
{
    if (m_connections.contains(deviceId)) {
        auto &conn = m_connections[deviceId];
        if (conn.socket) {
            conn.socket->disconnectFromHost();
            delete conn.socket;
        }
        m_connections.remove(deviceId);
        emit deviceDisconnected(deviceId);
        qDebug() << "Device" << deviceId << "disconnected";
    }
}

bool TcpCollector::isDeviceConnected(const QString &deviceId) const
{
    return m_connections.contains(deviceId) && m_connections[deviceId].connected;
}

void TcpCollector::onConnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString deviceId;
    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        if (it.value().socket == socket) {
            deviceId = it.key();
            break;
        }
    }

    if (deviceId.isEmpty()) return;

    m_connections[deviceId].connected = true;
    qDebug() << "Connected to TCP device" << deviceId;
    emit deviceConnected(deviceId);
}

void TcpCollector::onDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString deviceId;
    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        if (it.value().socket == socket) {
            deviceId = it.key();
            break;
        }
    }

    if (deviceId.isEmpty()) return;

    m_connections[deviceId].connected = false;
    qDebug() << "Disconnected from TCP device" << deviceId;
    emit deviceDisconnected(deviceId);
}

void TcpCollector::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString deviceId;
    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        if (it.value().socket == socket) {
            deviceId = it.key();
            break;
        }
    }

    if (deviceId.isEmpty()) return;

    auto &conn = m_connections[deviceId];
    conn.buffer.append(socket->readAll());

    qDebug() << "Data received from" << deviceId << ":" << conn.buffer.size() << "bytes";
}

void TcpCollector::onError(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString deviceId;
    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        if (it.value().socket == socket) {
            deviceId = it.key();
            break;
        }
    }

    if (deviceId.isEmpty()) return;

    QString error = socket->errorString();
    qWarning() << "TCP socket error for device" << deviceId << ":" << error;
    emit deviceError(deviceId, error);

    m_connections[deviceId].connected = false;
}

QString TcpCollector::sendCommand(QTcpSocket *socket, const QString &command, const QString &delimiter)
{
    if (!socket || !socket->isOpen()) {
        return QString();
    }

    // Отправляем команду
    QByteArray data = command.toUtf8();
    if (!delimiter.isEmpty()) {
        data += delimiter.toUtf8();
    }

    qDebug() << "Sending command:" << data;

    socket->write(data);
    if (!socket->waitForBytesWritten(1000)) {
        qDebug() << "Failed to write to socket";
        return QString();
    }

    // Читаем ответ
    if (!socket->waitForReadyRead(3000)) {
        qDebug() << "Timeout waiting for response";
        return QString();
    }

    QByteArray response = socket->readAll();
    return QString::fromUtf8(response);
}

void TcpCollector::poll()
{
    if (!m_enabled) {
        qDebug() << "TCP/IP Collector is disabled, skipping poll";
        return;
    }

    if (m_devices.isEmpty()) {
        qDebug() << "No TCP/IP devices configured for polling";
        return;
    }

    m_pollCount++;
    m_lastPollTime = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "TCP/IP poll #" << m_pollCount << "starting, devices:" << m_devices.size();

    int totalReadings = 0;

    for (const TcpDevice &device : m_devices) {
        if (!device.enabled || device.commands.isEmpty()) {
            continue;
        }

        // Проверяем валидность устройства
        if (device.host.isEmpty() || device.port <= 0) {
            qWarning() << "Invalid TCP device:" << device.deviceId;
            continue;
        }

        // Подключаемся если нужно
        if (!isDeviceConnected(device.deviceId)) {
            if (!connectDevice(device.deviceId)) {
                continue;
            }
            // Даём время на подключение
            QThread::msleep(100);
        }

        // Проверяем, что соединение существует и валидно
        if (!m_connections.contains(device.deviceId)) {
            qWarning() << "Connection not found for device:" << device.deviceId;
            continue;
        }

        auto &conn = m_connections[device.deviceId];
        if (!conn.connected || !conn.socket) {
            qDebug() << "Device" << device.deviceId << "not connected, skipping";
            continue;
        }

        // Проверяем, что сокет открыт
        if (conn.socket->state() != QAbstractSocket::ConnectedState) {
            qDebug() << "Socket not connected for device:" << device.deviceId;
            conn.connected = false;
            continue;
        }

        qDebug() << "Polling TCP device" << device.deviceId << "(" << device.host << ":" << device.port << ")";

        for (auto it = device.commands.begin(); it != device.commands.end(); ++it) {
            const QString &sensorId = it.key();
            const QString &command = it.value();

            // Проверяем валидность команды
            if (command.isEmpty()) {
                qWarning() << "Empty command for sensor:" << sensorId;
                continue;
            }

            try {
                QString response = sendCommand(conn.socket, command, device.delimiter);

                if (!response.isEmpty()) {
                    bool ok;
                    qreal value = response.toDouble(&ok);
                    if (ok) {
                        emit dataReceived(sensorId, value);
                        qDebug() << "  Sensor" << sensorId << "=" << value;
                        totalReadings++;
                    } else {
                        emit errorOccurred(sensorId, "Invalid response: " + response);
                        qDebug() << "  Error reading" << sensorId << ":" << response;
                    }
                } else {
                    emit errorOccurred(sensorId, "No response to command: " + command);
                    qDebug() << "  No response for" << sensorId;
                }
            } catch (...) {
                qWarning() << "Exception while polling sensor:" << sensorId;
                emit errorOccurred(sensorId, "Exception during poll");
            }
        }
    }

    emit pollCompleted(totalReadings);
    qDebug() << "TCP/IP poll completed, readings:" << totalReadings;
}
