#include "traymanager.h"
#include "utils/logger.h"
#include <QApplication>
#include <QPainter>
#include <QPixmap>

TrayManager::TrayManager(QObject *parent)
    : QObject(parent)
    , m_trayIcon(nullptr)
    , m_contextMenu(nullptr)
    , m_quitAction(nullptr)
{
    createContextMenu();
    createTrayIcon();
}

TrayManager::~TrayManager()
{
    if (m_trayIcon) {
        m_trayIcon->hide();
        delete m_trayIcon;
    }
    delete m_contextMenu;
}

bool TrayManager::isAvailable()
{
    return QSystemTrayIcon::isSystemTrayAvailable();
}

void TrayManager::show()
{
    if (m_trayIcon) {
        m_trayIcon->show();
        Logger::instance().info("Tray icon shown");
    }
}

void TrayManager::hide()
{
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
}

void TrayManager::showMessage(const QString &title, const QString &message,
                               QSystemTrayIcon::MessageIcon icon, int msecs)
{
    if (m_trayIcon) {
        m_trayIcon->showMessage(title, message, icon, msecs);
    }
}

void TrayManager::createContextMenu()
{
    m_contextMenu = new QMenu();

    // Пункт "Завершить"
    m_quitAction = new QAction(tr("Завершить"), m_contextMenu);
    connect(m_quitAction, &QAction::triggered, this, &TrayManager::quitRequested);
    m_contextMenu->addAction(m_quitAction);
}

void TrayManager::createTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);

    // Устанавливаем иконку
    m_trayIcon->setIcon(createDefaultIcon());

    // Устанавливаем всплывающую подсказку
    m_trayIcon->setToolTip(tr("Monitoring Server"));

    // Устанавливаем контекстное меню
    m_trayIcon->setContextMenu(m_contextMenu);

    // Подключаем сигнал активации (клик по иконке)
    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &TrayManager::onTrayIconActivated);
}

QIcon TrayManager::createDefaultIcon()
{
    // Создаём иконку программно: зелёный круг с буквой "M"
    // Это избавляет от необходимости хранить файл иконки
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Зелёный круг (фон)
    painter.setBrush(QColor(76, 175, 80));  // Material Green
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(2, 2, 60, 60);

    // Белая буква "M"
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(36);
    painter.setFont(font);
    painter.drawText(QRect(0, 0, 64, 64), Qt::AlignCenter, "M");

    painter.end();

    return QIcon(pixmap);
}

void TrayManager::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
            // Одиночный клик — показываем меню (на некоторых ОС это происходит автоматически)
            Logger::instance().debug("Tray icon clicked");
            break;

        case QSystemTrayIcon::DoubleClick:
            // Двойной клик — можно открыть окно настроек (в будущем)
            Logger::instance().debug("Tray icon double-clicked");
            break;

        default:
            break;
    }
}
