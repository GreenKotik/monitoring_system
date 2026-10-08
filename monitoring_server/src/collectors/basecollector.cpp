#include "collectors/basecollector.h"
#include <QDebug>

BaseCollector::BaseCollector(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    m_timer = new QTimer(this);
    // Убеждаемся, что таймер не запускается до явного вызова start
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, &BaseCollector::poll);
    qDebug() << "BaseCollector created:" << name;
}

void BaseCollector::start(int intervalMs)
{
    if (m_isRunning) {
        qDebug() << "Collector" << m_name << "already running";
        return;
    }

    m_interval = intervalMs;
    m_isRunning = true;

    // Проверяем, что таймер валидный
    if (!m_timer) {
        qCritical() << "Timer is null for collector" << m_name;
        m_isRunning = false;
        return;
    }

    m_timer->start(m_interval);

    emit statusChanged("Running");
    emit started();

    qDebug() << "Collector" << m_name << "started with interval" << m_interval << "ms";
}

void BaseCollector::stop()
{
    if (!m_isRunning) {
        qDebug() << "Collector" << m_name << "not running";
        return;
    }

    m_isRunning = false;

    if (m_timer) {
        m_timer->stop();
    }

    emit statusChanged("Stopped");
    emit stopped();

    qDebug() << "Collector" << m_name << "stopped";
}
