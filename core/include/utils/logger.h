#pragma once

#include <QObject>
#include <QDateTime>
#include <QDebug>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

class Logger : public QObject
{
    Q_OBJECT

private:
    explicit Logger(QObject *parent = nullptr);
    ~Logger();

public:
    static Logger& instance();

    void init(const QString &logFile = "app.log", LogLevel level = LogLevel::Info);
    void setLogLevel(LogLevel level);
    LogLevel logLevel() const;

    void debug(const QString &message);
    void info(const QString &message);
    void warning(const QString &message);
    void error(const QString &message);
    void critical(const QString &message);

    void log(LogLevel level, const QString &message);

private:
    QString levelToString(LogLevel level) const;

    LogLevel m_level;
    bool m_initialized;
};
