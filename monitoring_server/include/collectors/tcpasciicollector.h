#ifndef TCPASCIICOLLECTOR_H
#define TCPASCIICOLLECTOR_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QString>
#include <QRegularExpression>
#include "collectors/tcpasciiclient.h"

class TcpAsciiCollector : public QObject
{
    Q_OBJECT

public:
    // Конфигурация одного датчика внутри устройства
    struct TcpSensor {
        QString sensorId;
        QString command;           // Команда для запроса (например, "READ:TEMP?")
        QByteArray terminator;     // Терминатор команды (CR, LF)
        QByteArray responseTerm;   // Терминатор ответа
        QString regex;             // Регулярное выражение для парсинга значения
        int timeoutMs;
    };

    // Конфигурация устройства (TCP-хоста)
    struct TcpDevice {
        QString deviceId;
        QString host;
        int port;
        ChecksumType checksumType; // Тип контрольной суммы
        QList<TcpSensor> sensors;  // Список датчиков на этом устройстве
        bool enabled;
    };

    explicit TcpAsciiCollector(QObject *parent = nullptr);
    ~TcpAsciiCollector();

    void addDevice(const TcpDevice &device);
    void removeDevice(const QString &deviceId);
    void clearDevices();

    void start(int intervalMs);
    void stop();
    void pollNow();

    QList<QString> deviceIds() const;
    bool isRunning() const;

signals:
    void dataReceived(const QString &sensorId, qreal value, const QDateTime &timestamp);
    void errorOccurred(const QString &deviceId, const QString &error);
    void pollCompleted(int deviceCount);

private slots:
    void onPollTimerTimeout();

private:
    void pollDevice(const TcpDevice &device);
    qreal parseResponse(const QByteArray &response, const QString &regex);

    QMap<QString, TcpDevice> m_devices;
    QTimer *m_pollTimer;
    bool m_isRunning;
};

#endif // TCPASCIICOLLECTOR_H
