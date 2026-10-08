#ifndef MODBUSCOLLECTOR_H
#define MODBUSCOLLECTOR_H

#include "basecollector.h"
#include <QMap>
#include <QString>
#include <QList>
#include <QSerialPort>
#include <QTimer>

class ModbusCollector : public BaseCollector
{
    Q_OBJECT

public:
    struct ModbusRegister {
        QString sensorId;
        int address;
        int count = 1;
        int type = 3; // 1 - coil, 2 - discrete input, 3 - holding register, 4 - input register
        qreal scale = 1.0;
        qreal offset = 0.0;
        QString format = "uint16"; // uint16, int16, uint32, int32, float
        bool enabled = true;
    };

    struct ModbusDevice {
        QString deviceId;
        QString portName;
        int slaveId = 1;
        int baudRate = 9600;
        QSerialPort::DataBits dataBits = QSerialPort::Data8;
        QSerialPort::Parity parity = QSerialPort::NoParity;
        QSerialPort::StopBits stopBits = QSerialPort::OneStop;
        QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
        int timeout = 1000;
        QList<ModbusRegister> registers;
        bool enabled = true;
    };

    explicit ModbusCollector(QObject *parent = nullptr);
    ~ModbusCollector();

    void addDevice(const ModbusDevice &device);
    void removeDevice(const QString &deviceId);
    QList<ModbusDevice> devices() const { return m_devices; }
    bool hasDevice(const QString &deviceId) const;
    void clearDevices();

    void poll() override;
    void setEnabled(bool enabled) override;

signals:
    void deviceAdded(const QString &deviceId);
    void deviceRemoved(const QString &deviceId);
    void deviceError(const QString &deviceId, const QString &error);
    void portOpened(const QString &deviceId);
    void portClosed(const QString &deviceId);

private:
    bool openPort(ModbusDevice &device, QSerialPort &port);
    void closePort(QSerialPort &port);
    qreal readRegister(QSerialPort &port, const ModbusRegister &reg, int slaveId);
    QByteArray buildModbusFrame(int slaveId, int functionCode, int address, int count);
    bool parseModbusResponse(const QByteArray &data, int slaveId, int functionCode, QByteArray &responseData);

    QList<ModbusDevice> m_devices;
    QMap<QString, QSerialPort*> m_ports;
    int m_pollCount = 0;
};

#endif // MODBUSCOLLECTOR_H