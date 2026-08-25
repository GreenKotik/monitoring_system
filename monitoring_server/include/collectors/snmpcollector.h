#ifndef SNMPCOLLECTOR_H
#define SNMPCOLLECTOR_H

#include "basecollector.h"
#include <QMap>
#include <QString>

class SnmpCollector : public BaseCollector
{
    Q_OBJECT

public:
    struct SnmpDevice {
        QString deviceId;
        QString host;
        int port = 161;
        QString community = "public";
        int version = 2; // 1, 2c, 3
        QMap<QString, QString> oids; // sensorId -> OID
    };

    explicit SnmpCollector(QObject *parent = nullptr);

    void addDevice(const SnmpDevice &device);
    void removeDevice(const QString &deviceId);
    QList<SnmpDevice> devices() const { return m_devices; }

    void poll() override;

private:
    qreal getValue(const QString &host, int port, const QString &oid, const QString &community);

    QList<SnmpDevice> m_devices;
};

#endif // SNMPCOLLECTOR_H