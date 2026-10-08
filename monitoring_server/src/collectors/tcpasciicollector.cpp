#include "collectors/tcpasciicollector.h"
#include "utils/logger.h"
#include <QDebug>
#include <QDateTime>
#include <QRegularExpressionMatch>

TcpAsciiCollector::TcpAsciiCollector(QObject *parent)
    : QObject(parent)
    , m_pollTimer(new QTimer(this))
    , m_isRunning(false)
{
    connect(m_pollTimer, &QTimer::timeout, this, &TcpAsciiCollector::onPollTimerTimeout);
}

TcpAsciiCollector::~TcpAsciiCollector()
{
    stop();
}

void TcpAsciiCollector::addDevice(const TcpDevice &device)
{
    if (device.deviceId.isEmpty()) return;
    m_devices[device.deviceId] = device;
    Logger::instance().info(QString("TCP device added: %1 (%2:%3)")
                            .arg(device.deviceId).arg(device.host).arg(device.port));
}

void TcpAsciiCollector::removeDevice(const QString &deviceId)
{
    m_devices.remove(deviceId);
}

void TcpAsciiCollector::clearDevices()
{
    m_devices.clear();
}

void TcpAsciiCollector::start(int intervalMs)
{
    if (m_isRunning || m_devices.isEmpty()) return;
    m_pollTimer->start(intervalMs);
    m_isRunning = true;
    pollNow();
}

void TcpAsciiCollector::stop()
{
    if (!m_isRunning) return;
    m_pollTimer->stop();
    m_isRunning = false;
}

void TcpAsciiCollector::pollNow()
{
    int count = 0;
    for (auto it = m_devices.begin(); it != m_devices.end(); ++it) {
        if (it.value().enabled) {
            pollDevice(it.value());
            count++;
        }
    }
    emit pollCompleted(count);
}

QList<QString> TcpAsciiCollector::deviceIds() const { return m_devices.keys(); }
bool TcpAsciiCollector::isRunning() const { return m_isRunning; }

void TcpAsciiCollector::onPollTimerTimeout() { pollNow(); }

void TcpAsciiCollector::pollDevice(const TcpDevice &device)
{
    TcpAsciiClient client;

    // Подключаемся к устройству
    if (!client.connect(device.host, device.port, 3000)) {
        emit errorOccurred(device.deviceId, "Connection failed: " + client.lastError());
        return;
    }

    QDateTime timestamp = QDateTime::currentDateTime();

    // Опрашиваем каждый датчик на этом устройстве
    for (const TcpSensor &sensor : device.sensors) {
        // Формируем команду с контрольной суммой (если нужно)
        QByteArray cmd = sensor.command.toUtf8();
        if (device.checksumType != ChecksumType::None) {
            cmd = TcpAsciiClient::appendChecksum(cmd, device.checksumType);
        }

        // Отправляем и ждем ответ
        QByteArray response = client.sendAndWaitForTerminator(
            cmd, sensor.terminator, sensor.timeoutMs
        );

        if (response.isEmpty()) {
            emit errorOccurred(device.deviceId, QString("Timeout for sensor %1").arg(sensor.sensorId));
            continue;
        }

        // Парсим ответ регулярным выражением
        qreal value = parseResponse(response, sensor.regex);

        if (value != -999999.0) { // Магическое число ошибки
            emit dataReceived(sensor.sensorId, value, timestamp);
        } else {
            Logger::instance().warning(QString("Failed to parse TCP response for %1: %2")
                                       .arg(sensor.sensorId).arg(QString(response)));
        }
    }

    client.disconnect();
}

qreal TcpAsciiCollector::parseResponse(const QByteArray &response, const QString &regexStr)
{
    if (regexStr.isEmpty()) {
        // Если regex нет, пытаемся просто конвертировать ответ в число
        QString str = QString(response).trimmed();
        bool ok;
        qreal val = str.toDouble(&ok);
        return ok ? val : -999999.0;
    }

    QRegularExpression re(regexStr);
    QRegularExpressionMatch match = re.match(QString(response));

    if (match.hasMatch() && match.capturedTexts().size() > 1) {
        bool ok;
        qreal val = match.captured(1).toDouble(&ok);
        return ok ? val : -999999.0;
    }

    return -999999.0;
}
