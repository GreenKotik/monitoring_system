#include "utils/config.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

Config& Config::instance()
{
    static Config instance;
    return instance;
}

Config::Config()
{
    // Определяем путь к конфигурационному файлу
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty()) {
        configDir = QDir::homePath() + "/.config/monitoring_system";
    }

    QDir dir(configDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    m_configFile = configDir + "/monitoring_system.conf";
}

bool Config::load(const QString &filePath)
{
    if (!filePath.isEmpty()) {
        m_configFile = filePath;
    }

    QFile file(m_configFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Config file not found, using defaults:" << m_configFile;
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "Invalid JSON in config file";
        return false;
    }

    m_settings = doc.object();
    file.close();

    qDebug() << "Config loaded from:" << m_configFile;
    return true;
}

bool Config::save(const QString &filePath)
{
    QString path = filePath.isEmpty() ? m_configFile : filePath;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot save config to:" << path;
        return false;
    }

    QJsonDocument doc(m_settings);
    file.write(doc.toJson());
    file.close();

    qDebug() << "Config saved to:" << path;
    return true;
}

QVariant Config::get(const QString &key, const QVariant &defaultValue) const
{
    // Исправлено: QJsonObject::value() принимает только один аргумент
    if (m_settings.contains(key)) {
        return m_settings.value(key).toVariant();
    }
    return defaultValue;
}

void Config::set(const QString &key, const QVariant &value)
{
    m_settings[key] = QJsonValue::fromVariant(value);
}

QStringList Config::keys() const
{
    return m_settings.keys();
}

void Config::remove(const QString &key)
{
    m_settings.remove(key);
}

void Config::clear()
{
    m_settings = QJsonObject();
}

bool Config::contains(const QString &key) const
{
    return m_settings.contains(key);
}

QString Config::configFile() const
{
    return m_configFile;
}
