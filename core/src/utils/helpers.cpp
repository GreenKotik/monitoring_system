#include "utils/helpers.h"
#include <QDateTime>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QUuid>
#include <cmath>  // Добавляем для qPow

bool Helpers::isValidEmail(const QString &email)
{
    static QRegularExpression regex(R"((^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$))");
    return regex.match(email).hasMatch();
}

bool Helpers::isValidObjectId(const QString &id)
{
    static QRegularExpression regex(R"(^[a-zA-Z0-9_-]+$)");
    return regex.match(id).hasMatch() && id.length() <= 50;
}

bool Helpers::isValidSensorId(const QString &id)
{
    static QRegularExpression regex(R"(^[a-zA-Z0-9_-]+$)");
    return regex.match(id).hasMatch() && id.length() <= 50;
}

QString Helpers::generateId(const QString &prefix)
{
    QString id = QUuid::createUuid().toString(QUuid::Id128);
    id.remove('{');
    id.remove('}');
    id.remove('-');
    return prefix + "_" + id.left(12);
}

QString Helpers::hashPassword(const QString &password)
{
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromUtf8(hash.toHex());
}

bool Helpers::verifyPassword(const QString &password, const QString &hash)
{
    return hashPassword(password) == hash;
}

QString Helpers::formatDateTime(const QDateTime &dt)
{
    return dt.toString("yyyy-MM-dd HH:mm:ss");
}

QString Helpers::formatDuration(qint64 milliseconds)
{
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    qint64 hours = minutes / 60;
    qint64 days = hours / 24;

    if (days > 0) {
        return QString("%1д %2ч").arg(days).arg(hours % 24);
    }
    if (hours > 0) {
        return QString("%1ч %2мин").arg(hours).arg(minutes % 60);
    }
    if (minutes > 0) {
        return QString("%1мин %2с").arg(minutes).arg(seconds % 60);
    }
    return QString("%1с").arg(seconds);
}

double Helpers::roundTo(double value, int decimals)
{
    double factor = std::pow(10, decimals);  // Используем std::pow вместо qPow
    return std::round(value * factor) / factor;
}

QString Helpers::bytesToHuman(qint64 bytes)
{
    if (bytes < 1024) {
        return QString("%1 B").arg(bytes);
    }
    if (bytes < 1024 * 1024) {
        return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    }
    if (bytes < 1024 * 1024 * 1024) {
        return QString("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 1);
    }
    return QString("%1 GB").arg(bytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 1);
}

bool Helpers::isNumeric(const QString &str)
{
    bool ok;
    str.toDouble(&ok);
    return ok;
}

QString Helpers::truncate(const QString &str, int maxLength)
{
    if (str.length() <= maxLength) {
        return str;
    }
    return str.left(maxLength - 3) + "...";
}
