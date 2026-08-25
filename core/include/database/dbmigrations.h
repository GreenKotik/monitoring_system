#ifndef DBMIGRATIONS_H
#define DBMIGRATIONS_H

#include <QString>
#include <QList>
#include <functional>
#include "core_global.h"

class CORE_EXPORT DbMigrations
{
public:
    struct Migration {
        int version;
        QString name;
        std::function<bool(QSqlQuery&)> up;
        std::function<bool(QSqlQuery&)> down;
    };

    static QList<Migration> getAllMigrations();
    static bool runMigrations();
    static bool rollbackTo(int version);

private:
    static bool createMigrationsTable();
    static int getCurrentVersion();
    static bool setVersion(int version);
};

#endif // DBMIGRATIONS_H