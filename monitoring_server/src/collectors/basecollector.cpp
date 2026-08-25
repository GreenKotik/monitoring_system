#include "collectors/basecollector.h"

BaseCollector::BaseCollector(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &BaseCollector::poll);
}

void BaseCollector::start(int intervalMs)
{
    if (m_isRunning) return;

    m_interval = intervalMs;
    m_isRunning = true;
    m_timer->start(m_interval);

    emit statusChanged("Running");
    emit started();

    qDebug() << "Collector" << m_name << "started with interval" << m_interval << "ms";
}

void BaseCollector::stop()
{
    if (!m_isRunning) return;

    m_isRunning = false;
    m_timer->stop();

    emit statusChanged("Stopped");
    emit stopped();

    qDebug() << "Collector" << m_name << "stopped";
}