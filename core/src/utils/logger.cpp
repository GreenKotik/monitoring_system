#include "utils/logger.h"
#include <QDebug>

Logger::Logger(QObject *parent)
    : QObject(parent)
    , m_level(LogLevel::Info)
    , m_initialized(false)
{
}

Logger::~Logger() {
}

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::init(const QString &logFile, LogLevel level) {
    // Просто выводим в консоль, без записи в файл
    m_level = level;
    m_initialized = true;
    qDebug() << "Logger initialized (console only)";
}

void Logger::setLogLevel(LogLevel level) {
    m_level = level;
}

LogLevel Logger::logLevel() const {
    return m_level;
}

void Logger::debug(const QString &message) {
    log(LogLevel::Debug, message);
}

void Logger::info(const QString &message) {
    log(LogLevel::Info, message);
}

void Logger::warning(const QString &message) {
    log(LogLevel::Warning, message);
}

void Logger::error(const QString &message) {
    log(LogLevel::Error, message);
}

void Logger::critical(const QString &message) {
    log(LogLevel::Critical, message);
}

void Logger::log(LogLevel level, const QString &message) {
    if (static_cast<int>(level) < static_cast<int>(m_level)) {
        return;
    }

    QString formatted = QString("%1 [%2] %3")
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"))
        .arg(levelToString(level))
        .arg(message);

    // Вывод в консоль
    switch (level) {
        case LogLevel::Debug:
            qDebug() << formatted;
            break;
        case LogLevel::Info:
            qInfo() << formatted;
            break;
        case LogLevel::Warning:
            qWarning() << formatted;
            break;
        case LogLevel::Error:
            qCritical() << formatted;
            break;
        case LogLevel::Critical:
            qCritical() << formatted;
            break;
    }
}

QString Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Critical:return "CRITICAL";
        default:                return "UNKNOWN";
    }
}
