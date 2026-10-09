#include "traymanager.h"
#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QDebug>

// ✅ НОВЫЙ конструктор с параметрами
TrayManager::TrayManager(const QString &appName,
                         const QString &letter,
                         const QColor &color,
                         QObject *parent)
    : QObject(parent)
    , m_trayIcon(nullptr)
    , m_contextMenu(nullptr)
    , m_quitAction(nullptr)
    , m_appName(appName)
    , m_letter(letter)
    , m_color(color)
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
    m_quitAction = new QAction(tr("Завершить"), m_contextMenu);
    connect(m_quitAction, &QAction::triggered, this, &TrayManager::quitRequested);
    m_contextMenu->addAction(m_quitAction);
}

void TrayManager::createTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);

    // ✅ Используем настраиваемую иконку
    m_trayIcon->setIcon(createIcon(m_letter, m_color));

    // ✅ Используем настраиваемое название
    m_trayIcon->setToolTip(m_appName);

    m_trayIcon->setContextMenu(m_contextMenu);

    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &TrayManager::onTrayIconActivated);
}

// ✅ НОВЫЙ метод: создаёт иконку с заданной буквой и цветом
QIcon TrayManager::createIcon(const QString &letter, const QColor &color)
{
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Круг заданного цвета
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(2, 2, 60, 60);

    // Белая буква
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(36);
    painter.setFont(font);
    painter.drawText(QRect(0, 0, 64, 64), Qt::AlignCenter, letter);

    painter.end();

    return QIcon(pixmap);
}

void TrayManager::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    Q_UNUSED(reason);
}
