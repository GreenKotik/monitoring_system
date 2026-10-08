#ifndef TCPCOLLECTOR_H
#define TCPCOLLECTOR_H

#include "basecollector.h"
#include <QTcpSocket>
#include <QMap>
#include <QString>
#include <QList>
#include <QTimer>

class TcpCollector : public BaseCollector
{
    Q_OBJECT

public:
    struct TcpDevice {
        QString deviceId;
        QString host;
        int port = 502;
        int timeout = 5000;
        QString delimiter = "\r\n";  // Разделитель сообщений
        QMap<QString, QString> commands; // sensorId -> команда для запроса
        bool enabled = true;
    };

    struct TcpMessage {
        QString sensorId;
        QString command;
        QString response;
        bool success;
        QString error;
    };

    explicit TcpCollector(QObject *parent = nullptr);
    ~TcpCollector();

    void addDevice(const TcpDevice &device);
    void removeDevice(const QString &deviceId);
    QList<TcpDevice> devices() const { return m_devices; }
    bool hasDevice(const QString &deviceId) const;
    void clearDevices();

    void poll() override;
    void setEnabled(bool enabled) override;

    // Подключение
    bool connectDevice(const QString &deviceId);
    void disconnectDevice(const QString &deviceId);
    bool isDeviceConnected(const QString &deviceId) const;

signals:
    void deviceAdded(const QString &deviceId);
    void deviceRemoved(const QString &deviceId);
    void deviceConnected(const QString &deviceId);
    void deviceDisconnected(const QString &deviceId);
    void deviceError(const QString &deviceId, const QString &error);
    void messageReceived(const QString &sensorId, const QString &response);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:
    struct TcpConnection {
        QTcpSocket *socket = nullptr;
        bool connected = false;
        QByteArray buffer;
        TcpDevice device;
    };

    QString sendCommand(QTcpSocket *socket, const QString &command, const QString &delimiter);
    QString parseResponse(const QByteArray &data, const QString &delimiter);

    QList<TcpDevice> m_devices;
    QMap<QString, TcpConnection> m_connections;
    int m_pollCount = 0;
};

#endif // TCPCOLLECTOR_H