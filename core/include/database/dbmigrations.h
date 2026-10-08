#pragma once

#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QList>

struct Migration {
    int version;
    QString name;
    QString upSql;
    QString downSql;
};

class DBMigrations
{
public:
    static DBMigrations& instance();

    bool apply();
    bool rollbackTo(int version);
    QString currentVersion() const;

private:
    DBMigrations();
    ~DBMigrations();

    bool createMigrationsTable();
    bool executeSql(const QString &sql);
    bool setVersion(int version);
    QList<Migration> getAllMigrations() const;

    QString m_currentVersion;
};
