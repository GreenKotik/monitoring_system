#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QColor>

class TrayManager : public QObject
{
    Q_OBJECT

public:
    // ✅ НОВЫЙ конструктор с настраиваемыми параметрами
    explicit TrayManager(const QString &appName = "Server",
                         const QString &letter = "S",
                         const QColor &color = QColor(76, 175, 80),  // Зелёный по умолчанию
                         QObject *parent = nullptr);
    ~TrayManager();

    void show();
    void hide();

    void showMessage(const QString &title, const QString &message,
                     QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information,
                     int msecs = 3000);

    static bool isAvailable();

signals:
    void quitRequested();

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createContextMenu();
    void createTrayIcon();
    QIcon createIcon(const QString &letter, const QColor &color);

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_contextMenu;
    QAction *m_quitAction;

    QString m_appName;    // Название приложения (для tooltip)
    QString m_letter;     // Буква на иконке
    QColor m_color;       // Цвет иконки
};

#endif // TRAYMANAGER_H
