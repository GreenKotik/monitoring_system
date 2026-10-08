#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QJsonObject>
#include <QVariant>
#include "../core_global.h"

class CORE_EXPORT Config
{
public:
    static Config& instance();

    bool load(const QString &filePath = QString());
    bool save(const QString &filePath = QString());

    QVariant get(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void set(const QString &key, const QVariant &value);
    
    QStringList keys() const;
    void remove(const QString &key);
    void clear();
    bool contains(const QString &key) const;
    
    QString configFile() const;

private:
    Config();
    QString m_configFile;
    QJsonObject m_settings;
};

#endif // CONFIG_H