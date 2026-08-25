#ifndef ADMIN_MAINWINDOW_H
#define ADMIN_MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QJsonArray>
#include "admin_apiclient.h"
#include "core/models/object.h"
#include "core/models/sensor.h"
#include "core/models/user.h"

class AdminMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminMainWindow(QWidget *parent = nullptr);
    ~AdminMainWindow();

private slots:
    // Общие
    void onLoginSuccess(const QString &token, const QString &username);
    void onLoginFailed(const QString &error);
    void onLogout();

    // Объекты
    void onObjectsLoaded(const QJsonArray &objects);
    void onObjectCreated(const QJsonObject &object);
    void onObjectUpdated(const QJsonObject &object);
    void onObjectDeleted(const QString &objectId);
    void onAddObject();
    void onEditObject();
    void onDeleteObject();
    void onRefreshObjects();

    // Датчики
    void onSensorsLoaded(const QJsonArray &sensors);
    void onSensorCreated(const QJsonObject &sensor);
    void onSensorUpdated(const QJsonObject &sensor);
    void onSensorDeleted(const QString &sensorId);
    void onAddSensor();
    void onEditSensor();
    void onDeleteSensor();
    void onRefreshSensors();

    // Пользователи
    void onUsersLoaded(const QJsonArray &users);
    void onUserCreated(const QJsonObject &user);
    void onUserUpdated(const QJsonObject &user);
    void onUserDeleted(const QString &userId);
    void onAddUser();
    void onEditUser();
    void onDeleteUser();
    void onRefreshUsers();

private:
    void setupUi();
    void setupConnections();
    void showLoginDialog();
    void updateStatusBar();
    void loadData();

    // Вкладки
    void setupObjectsTab();
    void setupSensorsTab();
    void setupUsersTab();

    // Таблицы
    void populateObjectsTable(const QList<Object> &objects);
    void populateSensorsTable(const QList<Sensor> &sensors);
    void populateUsersTable(const QList<User> &users);

    // UI компоненты
    QTabWidget *m_tabWidget;
    QToolBar *m_toolBar;

    // Вкладка объектов
    QWidget *m_objectsTab;
    QTableWidget *m_objectsTable;
    QPushButton *m_addObjectBtn;
    QPushButton *m_editObjectBtn;
    QPushButton *m_deleteObjectBtn;
    QPushButton *m_refreshObjectsBtn;

    // Вкладка датчиков
    QWidget *m_sensorsTab;
    QTableWidget *m_sensorsTable;
    QPushButton *m_addSensorBtn;
    QPushButton *m_editSensorBtn;
    QPushButton *m_deleteSensorBtn;
    QPushButton *m_refreshSensorsBtn;

    // Вкладка пользователей
    QWidget *m_usersTab;
    QTableWidget *m_usersTable;
    QPushButton *m_addUserBtn;
    QPushButton *m_editUserBtn;
    QPushButton *m_deleteUserBtn;
    QPushButton *m_refreshUsersBtn;

    AdminApiClient *m_apiClient;
    QList<Object> m_objects;
    QList<Sensor> m_sensors;
    QList<User> m_users;
};

#endif // ADMIN_MAINWINDOW_H