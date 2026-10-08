#include "collectors/modbuscollector.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QThread>
#include <QByteArray>
#include <QDataStream>

// CRC16 для Modbus
static quint16 calculateModbusCRC(const QByteArray &data)
{
    quint16 crc = 0xFFFF;
    for (int i = 0; i < data.size(); ++i) {
        crc ^= (quint8)data[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

ModbusCollector::ModbusCollector(QObject *parent)
    : BaseCollector("Modbus Collector", parent)
{
    qDebug() << "Modbus Collector initialized";
}

ModbusCollector::~ModbusCollector()
{
    stop();
    for (auto port : m_ports.values()) {
        if (port->isOpen()) {
            port->close();
        }
        delete port;
    }
    m_ports.clear();
}

void ModbusCollector::addDevice(const ModbusDevice &device)
{
    if (device.deviceId.isEmpty()) {
        qWarning() << "Cannot add device with empty ID";
        return;
    }

    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i].deviceId == device.deviceId) {
            qDebug() << "Device" << device.deviceId << "already exists, updating";
            m_devices[i] = device;
            emit deviceAdded(device.deviceId);
            return;
        }
    }

    m_devices.append(device);
    qDebug() << "Device" << device.deviceId << "added to Modbus collector";
    emit deviceAdded(device.deviceId);
}

void ModbusCollector::removeDevice(const QString &deviceId)
{
    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i].deviceId == deviceId) {
            // Закрываем порт если открыт
            if (m_ports.contains(deviceId)) {
                QSerialPort *port = m_ports[deviceId];
                if (port->isOpen()) {
                    port->close();
                }
                delete port;
                m_ports.remove(deviceId);
            }
            m_devices.removeAt(i);
            qDebug() << "Device" << deviceId << "removed from Modbus collector";
            emit deviceRemoved(deviceId);
            return;
        }
    }
    qWarning() << "Device" << deviceId << "not found";
}

bool ModbusCollector::hasDevice(const QString &deviceId) const
{
    for (const ModbusDevice &dev : m_devices) {
        if (dev.deviceId == deviceId) {
            return true;
        }
    }
    return false;
}

void ModbusCollector::clearDevices()
{
    for (auto port : m_ports.values()) {
        if (port->isOpen()) {
            port->close();
        }
        delete port;
    }
    m_ports.clear();
    m_devices.clear();
    qDebug() << "All Modbus devices cleared";
}

void ModbusCollector::setEnabled(bool enabled)
{
    BaseCollector::setEnabled(enabled);
    if (!enabled) {
        stop();
    }
}

void ModbusCollector::poll()
{
    if (!m_enabled) {
        qDebug() << "Modbus Collector is disabled, skipping poll";
        return;
    }

    if (m_devices.isEmpty()) {
        qDebug() << "No Modbus devices configured for polling";
        return;
    }

    m_pollCount++;
    m_lastPollTime = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "Modbus poll #" << m_pollCount << "starting, devices:" << m_devices.size();

    int totalReadings = 0;

    for (ModbusDevice &device : m_devices) {
        if (!device.enabled || device.registers.isEmpty()) {
            continue;
        }

        // Получаем или создаём порт для устройства
        QSerialPort *port = m_ports.value(device.deviceId, nullptr);
        if (!port) {
            port = new QSerialPort(this);
            m_ports[device.deviceId] = port;
        }

        if (!port->isOpen()) {
            if (!openPort(device, *port)) {
                continue;
            }
        }

        qDebug() << "Polling Modbus device" << device.deviceId << "on port" << device.portName;

        for (const ModbusRegister &reg : device.registers) {
            if (!reg.enabled) continue;

            qreal value = readRegister(*port, reg, device.slaveId);
            if (value >= 0) {
                emit dataReceived(reg.sensorId, value);
                qDebug() << "  Register" << reg.address << "=" << value;
                totalReadings++;
            } else {
                emit errorOccurred(reg.sensorId, "Failed to read register " + QString::number(reg.address));
            }
        }
    }

    emit pollCompleted(totalReadings);
    qDebug() << "Modbus poll completed, readings:" << totalReadings;
}

bool ModbusCollector::openPort(ModbusDevice &device, QSerialPort &port)
{
    port.setPortName(device.portName);
    port.setBaudRate(device.baudRate);
    port.setDataBits(device.dataBits);
    port.setParity(device.parity);
    port.setStopBits(device.stopBits);
    port.setFlowControl(device.flowControl);

    if (!port.open(QIODevice::ReadWrite)) {
        QString error = "Failed to open port " + device.portName + ": " + port.errorString();
        qWarning() << error;
        emit deviceError(device.deviceId, error);
        return false;
    }

    qDebug() << "Opened port" << device.portName << "for device" << device.deviceId;
    emit portOpened(device.deviceId);
    return true;
}

void ModbusCollector::closePort(QSerialPort &port)
{
    if (port.isOpen()) {
        port.close();
        qDebug() << "Closed port" << port.portName();
    }
}

qreal ModbusCollector::readRegister(QSerialPort &port, const ModbusRegister &reg, int slaveId)
{
    QByteArray request = buildModbusFrame(slaveId, reg.type, reg.address, reg.count);

    // Отправляем запрос
    port.write(request);
    if (!port.waitForBytesWritten(500)) {
        qWarning() << "Modbus write timeout for address" << reg.address;
        return -1;
    }

    // Читаем ответ
    QByteArray response;
    if (!port.waitForReadyRead(1000)) {
        qWarning() << "Modbus read timeout for address" << reg.address;
        return -1;
    }

    response = port.readAll();

    // Проверяем CRC и парсим данные
    QByteArray data;
    if (!parseModbusResponse(response, slaveId, reg.type, data)) {
        return -1;
    }

    // Парсим данные в зависимости от формата
    QDataStream stream(data);
    stream.setByteOrder(QDataStream::BigEndian);

    qreal value = 0;

    if (reg.format == "uint16") {
        quint16 val;
        stream >> val;
        value = val;
    } else if (reg.format == "int16") {
        qint16 val;
        stream >> val;
        value = val;
    } else if (reg.format == "uint32") {
        quint32 val;
        stream >> val;
        value = val;
    } else if (reg.format == "int32") {
        qint32 val;
        stream >> val;
        value = val;
    } else if (reg.format == "float") {
        float val;
        stream >> val;
        value = val;
    } else {
        // uint16 по умолчанию
        quint16 val;
        stream >> val;
        value = val;
    }

    // Применяем масштаб и смещение
    value = value * reg.scale + reg.offset;

    return value;
}

QByteArray ModbusCollector::buildModbusFrame(int slaveId, int functionCode, int address, int count)
{
    QByteArray frame;

    // Адрес устройства
    frame.append((char)slaveId);

    // Код функции
    frame.append((char)functionCode);

    // Адрес старшего байта
    frame.append((char)((address >> 8) & 0xFF));

    // Адрес младшего байта
    frame.append((char)(address & 0xFF));

    // Количество регистров старший байт
    frame.append((char)((count >> 8) & 0xFF));

    // Количество регистров младший байт
    frame.append((char)(count & 0xFF));

    // CRC
    quint16 crc = calculateModbusCRC(frame);
    frame.append((char)(crc & 0xFF));
    frame.append((char)((crc >> 8) & 0xFF));

    return frame;
}

bool ModbusCollector::parseModbusResponse(const QByteArray &data, int slaveId, int functionCode, QByteArray &responseData)
{
    if (data.size() < 5) {
        qWarning() << "Modbus response too short:" << data.size();
        return false;
    }

    // Проверяем адрес устройства
    if ((quint8)data[0] != slaveId) {
        qWarning() << "Modbus slave ID mismatch:" << (quint8)data[0] << "!=" << slaveId;
        return false;
    }

    // Проверяем код функции
    if ((quint8)data[1] != functionCode) {
        if ((quint8)data[1] == (functionCode | 0x80)) {
            qWarning() << "Modbus exception response, code:" << (quint8)data[2];
            return false;
        }
        qWarning() << "Modbus function code mismatch:" << (quint8)data[1] << "!=" << functionCode;
        return false;
    }

    // Проверяем CRC
    if (data.size() >= 5) {
        QByteArray frameWithoutCRC = data.left(data.size() - 2);
        quint16 crcReceived = ((quint8)data[data.size() - 2] | ((quint8)data[data.size() - 1] << 8));
        quint16 crcCalculated = calculateModbusCRC(frameWithoutCRC);
        if (crcReceived != crcCalculated) {
            qWarning() << "Modbus CRC mismatch:" << crcReceived << "!=" << crcCalculated;
            return false;
        }
    }

    // Извлекаем данные
    int dataLength = (quint8)data[2];
    if (data.size() < dataLength + 5) {
        qWarning() << "Modbus response data incomplete";
        return false;
    }

    responseData = data.mid(3, dataLength);
    return true;
}