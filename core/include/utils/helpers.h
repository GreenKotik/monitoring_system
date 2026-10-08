#ifndef HELPERS_H
#define HELPERS_H

#include <QString>
#include <QDateTime>
#include <QList>
#include "../core_global.h"

class CORE_EXPORT Helpers
{
public:
    static bool isValidEmail(const QString &email);
    static bool isValidObjectId(const QString &id);
    static bool isValidSensorId(const QString &id);
    static QString generateId(const QString &prefix = QString());
    static QString hashPassword(const QString &password);
    static bool verifyPassword(const QString &password, const QString &hash);
    static QString formatDateTime(const QDateTime &dt);
    static QString formatDuration(qint64 milliseconds);
    static double roundTo(double value, int decimals = 2);
    static QString bytesToHuman(qint64 bytes);
    static bool isNumeric(const QString &str);
    static QString truncate(const QString &str, int maxLength = 50);
};

#endif // HELPERS_H