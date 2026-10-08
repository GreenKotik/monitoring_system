#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

class TrayManager : public QObject
{
    Q_OBJECT

public:
    explicit TrayManager(QObject *parent = nullptr);
    ~TrayManager();

    // Показать иконку в трее
    void show();

    // Скрыть иконку
    void hide();

    // Показать уведомление в трее
    void showMessage(const QString &title, const QString &message,
                     QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information,
                     int msecs = 3000);

    // Проверить, доступен ли системный трей
    static bool isAvailable();

signals:
    // Сигнал, когда пользователь нажал "Завершить"
    void quitRequested();

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createTrayIcon();
    void createContextMenu();
    QIcon createDefaultIcon();

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_contextMenu;
    QAction *m_quitAction;
};

#endif // TRAYMANAGER_H
