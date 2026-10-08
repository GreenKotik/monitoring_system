#include "database/dbmigrations.h"
#include "database/dbmanager.h"
#include <QDebug>
#include <QUuid>

DBMigrations::DBMigrations() {}
DBMigrations::~DBMigrations() {}

DBMigrations& DBMigrations::instance() {
    static DBMigrations instance;
    return instance;
}

bool DBMigrations::apply() {
    qDebug() << "Applying migrations...";

    if (!createMigrationsTable()) {
        qCritical() << "Failed to create migrations table";
        return false;
    }

    QList<Migration> migrations = getAllMigrations();
    // ИСПРАВЛЕНО: используем другое имя переменной
    int currentVer = currentVersion().toInt();

    for (const Migration &migration : migrations) {
        if (migration.version > currentVer) {
            qDebug() << "Applying migration" << migration.version << "-" << migration.name;
            if (!executeSql(migration.upSql)) {
                qCritical() << "Failed to apply migration" << migration.version;
                return false;
            }
            if (!setVersion(migration.version)) {
                qCritical() << "Failed to update version to" << migration.version;
                return false;
            }
            currentVer = migration.version;
        }
    }

    qDebug() << "All migrations applied successfully. Current version:" << currentVer;
    return true;
}

bool DBMigrations::rollbackTo(int version) {
    qDebug() << "Rolling back to version" << version;

    // ИСПРАВЛЕНО: используем другое имя переменной
    int currentVer = currentVersion().toInt();
    QList<Migration> migrations = getAllMigrations();

    for (int i = migrations.size() - 1; i >= 0; --i) {
        const Migration &migration = migrations[i];
        if (migration.version > version && migration.version <= currentVer) {
            qDebug() << "Rolling back migration" << migration.version << "-" << migration.name;
            if (!executeSql(migration.downSql)) {
                qCritical() << "Failed to rollback migration" << migration.version;
                return false;
            }
            if (!setVersion(migration.version - 1)) {
                qCritical() << "Failed to update version to" << migration.version - 1;
                return false;
            }
            currentVer = migration.version - 1;
        }
    }

    qDebug() << "Rollback completed. Current version:" << currentVersion();
    return true;
}

QString DBMigrations::currentVersion() const {
    QSqlQuery query;
    query.prepare("SELECT version FROM schema_migrations ORDER BY version DESC LIMIT 1");

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return "0";
}

bool DBMigrations::createMigrationsTable() {
    QString sql = "CREATE TABLE IF NOT EXISTS schema_migrations ("
                  "version INTEGER PRIMARY KEY, "
                  "applied_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";
    return executeSql(sql);
}

bool DBMigrations::setVersion(int version) {
    QSqlQuery query;
    query.prepare("INSERT INTO schema_migrations (version) VALUES (?)");
    query.addBindValue(version);
    return query.exec();
}

QList<Migration> DBMigrations::getAllMigrations() const {
    QList<Migration> migrations;

    // Начальная миграция
    Migration initial;
    initial.version = 1;
    initial.name = "Initial schema";
    initial.upSql =
        "CREATE TABLE IF NOT EXISTS objects ("
        "  id TEXT PRIMARY KEY,"
        "  name TEXT NOT NULL,"
        "  object_type_id TEXT,"
        "  parent_object_id TEXT,"
        "  description TEXT,"
        "  position_x REAL DEFAULT 0,"
        "  position_y REAL DEFAULT 0,"
        "  size_width REAL DEFAULT 100,"
        "  size_height REAL DEFAULT 80,"
        "  color_code TEXT DEFAULT '#333333',"
        "  svg_scheme_path TEXT,"
        "  is_active INTEGER DEFAULT 1,"
        "  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "  updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE TABLE IF NOT EXISTS sensors ("
        "  id TEXT PRIMARY KEY,"
        "  name TEXT NOT NULL,"
        "  sensor_type_id TEXT,"
        "  object_id TEXT,"
        "  unit TEXT,"
        "  min_value REAL DEFAULT 0,"
        "  max_value REAL DEFAULT 100,"
        "  last_value REAL DEFAULT 0,"
        "  status TEXT DEFAULT 'normal',"
        "  polling_interval INTEGER DEFAULT 60,"
        "  connection_params TEXT,"
        "  is_active INTEGER DEFAULT 1,"
        "  last_update TIMESTAMP,"
        "  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE TABLE IF NOT EXISTS sensor_readings ("
        "  id TEXT PRIMARY KEY,"
        "  sensor_id TEXT,"
        "  value REAL NOT NULL,"
        "  quality TEXT DEFAULT 'normal',"
        "  timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE TABLE IF NOT EXISTS users ("
        "  id TEXT PRIMARY KEY,"
        "  username TEXT UNIQUE NOT NULL,"
        "  password_hash TEXT NOT NULL,"
        "  role TEXT DEFAULT 'user',"
        "  email TEXT,"
        "  is_active INTEGER DEFAULT 1,"
        "  last_login TIMESTAMP,"
        "  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE TABLE IF NOT EXISTS object_types ("
        "  id TEXT PRIMARY KEY,"
        "  name TEXT NOT NULL,"
        "  color_code TEXT DEFAULT '#333333',"
        "  icon_path TEXT,"
        "  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE TABLE IF NOT EXISTS sensor_types ("
        "  id TEXT PRIMARY KEY,"
        "  name TEXT NOT NULL,"
        "  unit TEXT,"
        "  min_value REAL,"
        "  max_value REAL,"
        "  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_sensor_readings_sensor_id ON sensor_readings(sensor_id);"
        "CREATE INDEX IF NOT EXISTS idx_sensor_readings_timestamp ON sensor_readings(timestamp);"
        "CREATE INDEX IF NOT EXISTS idx_objects_parent ON objects(parent_object_id);"
        "CREATE INDEX IF NOT EXISTS idx_sensors_object ON sensors(object_id);";

    initial.downSql =
        "DROP TABLE IF EXISTS sensor_readings;"
        "DROP TABLE IF EXISTS sensors;"
        "DROP TABLE IF EXISTS objects;"
        "DROP TABLE IF EXISTS users;"
        "DROP TABLE IF EXISTS object_types;"
        "DROP TABLE IF EXISTS sensor_types;";

    migrations.append(initial);
    return migrations;
}

bool DBMigrations::executeSql(const QString &sql) {
    QSqlQuery query;
    QStringList statements = sql.split(';', Qt::SkipEmptyParts);

    for (const QString &stmt : statements) {
        QString trimmed = stmt.trimmed();
        if (trimmed.isEmpty()) continue;

        if (!query.exec(trimmed)) {
            qCritical() << "SQL Error:" << query.lastError().text();
            qCritical() << "Statement:" << trimmed;
            return false;
        }
    }
    return true;
}
