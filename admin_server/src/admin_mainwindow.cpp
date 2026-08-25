#include "admin_mainwindow.h"
#include "dialogs/objectdialog.h"
#include "dialogs/sensordialog.h"
#include "dialogs/userdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

AdminMainWindow::AdminMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_apiClient = new AdminApiClient(this);
    m_apiClient->setBaseUrl("http://localhost:3000/api");

    setupUi();
    setupConnections();

    showLoginDialog();
}

AdminMainWindow::~AdminMainWindow()
{
}

void AdminMainWindow::setupUi()
{
    setWindowTitle("Администрирование системы мониторинга");
    setMinimumSize(1000, 700);

    // Создаём центральный виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Панель инструментов
    m_toolBar = new QToolBar(this);
    m_toolBar->addAction("Выход", this, &AdminMainWindow::onLogout);
    addToolBar(m_toolBar);

    // Вкладки
    m_tabWidget = new QTabWidget(this);

    setupObjectsTab();
    setupSensorsTab();
    setupUsersTab();

    mainLayout->addWidget(m_tabWidget);

    // Статус бар
    statusBar()->showMessage("Администрирование");
}

void AdminMainWindow::setupObjectsTab()
{
    m_objectsTab = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(m_objectsTab);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_addObjectBtn = new QPushButton("Добавить", this);
    m_editObjectBtn = new QPushButton("Изменить", this);
    m_deleteObjectBtn = new QPushButton("Удалить", this);
    m_refreshObjectsBtn = new QPushButton("Обновить", this);

    buttonLayout->addWidget(m_addObjectBtn);
    buttonLayout->addWidget(m_editObjectBtn);
    buttonLayout->addWidget(m_deleteObjectBtn);
    buttonLayout->addWidget(m_refreshObjectsBtn);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);

    // Таблица объектов
    m_objectsTable = new QTableWidget(this);
    m_objectsTable->setColumnCount(5);
    m_objectsTable->setHorizontalHeaderLabels({"ID", "Название", "Тип", "Статус", "Родитель"});
    m_objectsTable->horizontalHeader()->setStretchLastSection(true);
    m_objectsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_objectsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(m_objectsTable);

    m_tabWidget->addTab(m_objectsTab, "Объекты");
}

void AdminMainWindow::setupSensorsTab()
{
    m_sensorsTab = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(m_sensorsTab);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_addSensorBtn = new QPushButton("Добавить", this);
    m_editSensorBtn = new QPushButton("Изменить", this);
    m_deleteSensorBtn = new QPushButton("Удалить", this);
    m_refreshSensorsBtn = new QPushButton("Обновить", this);

    buttonLayout->addWidget(m_addSensorBtn);
    buttonLayout->addWidget(m_editSensorBtn);
    buttonLayout->addWidget(m_deleteSensorBtn);
    buttonLayout->addWidget(m_refreshSensorsBtn);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);

    // Таблица датчиков
    m_sensorsTable = new QTableWidget(this);
    m_sensorsTable->setColumnCount(6);
    m_sensorsTable->setHorizontalHeaderLabels({"ID", "Название", "Тип", "Объект", "Значение", "Статус"});
    m_sensorsTable->horizontalHeader()->setStretchLastSection(true);
    m_sensorsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_sensorsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(m_sensorsTable);

    m_tabWidget->addTab(m_sensorsTab, "Датчики");
}

void AdminMainWindow::setupUsersTab()
{
    m_usersTab = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(m_usersTab);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_addUserBtn = new QPushButton("Добавить", this);
    m_editUserBtn = new QPushButton("Изменить", this);
    m_deleteUserBtn = new QPushButton("Удалить", this);
    m_refreshUsersBtn = new QPushButton("Обновить", this);

    buttonLayout->addWidget(m_addUserBtn);
    buttonLayout->addWidget(m_editUserBtn);
    buttonLayout->addWidget(m_deleteUserBtn);
    buttonLayout->addWidget(m_refreshUsersBtn);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);

    // Таблица пользователей
    m_usersTable = new QTableWidget(this);
    m_usersTable->setColumnCount(4);
    m_usersTable->setHorizontalHeaderLabels({"ID", "Имя пользователя", "Email", "Роль"});
    m_usersTable->horizontalHeader()->setStretchLastSection(true);
    m_usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(m_usersTable);

    m_tabWidget->addTab(m_usersTab, "Пользователи");
}

void AdminMainWindow::setupConnections()
{
    connect(m_apiClient, &AdminApiClient::loginSuccess, this, &AdminMainWindow::onLoginSuccess);
    connect(m_apiClient, &AdminApiClient::loginFailed, this, &AdminMainWindow::onLoginFailed);

    connect(m_apiClient, &AdminApiClient::objectsReceived, this, &AdminMainWindow::onObjectsLoaded);
    connect(m_apiClient, &AdminApiClient::objectCreated, this, &AdminMainWindow::onObjectCreated);
    connect(m_apiClient, &AdminApiClient::objectUpdated, this, &AdminMainWindow::onObjectUpdated);
    connect(m_apiClient, &AdminApiClient::objectDeleted, this, &AdminMainWindow::onObjectDeleted);

    connect(m_apiClient, &AdminApiClient::sensorsReceived, this, &AdminMainWindow::onSensorsLoaded);
    connect(m_apiClient, &AdminApiClient::sensorCreated, this, &AdminMainWindow::onSensorCreated);
    connect(m_apiClient, &AdminApiClient::sensorUpdated, this, &AdminMainWindow::onSensorUpdated);
    connect(m_apiClient, &AdminApiClient::sensorDeleted, this, &AdminMainWindow::onSensorDeleted);

    connect(m_apiClient, &AdminApiClient::usersReceived, this, &AdminMainWindow::onUsersLoaded);
    connect(m_apiClient, &AdminApiClient::userCreated, this, &AdminMainWindow::onUserCreated);
    connect(m_apiClient, &AdminApiClient::userUpdated, this, &AdminMainWindow::onUserUpdated);
    connect(m_apiClient, &AdminApiClient::userDeleted, this, &AdminMainWindow::onUserDeleted);

    // Объекты
    connect(m_addObjectBtn, &QPushButton::clicked, this, &AdminMainWindow::onAddObject);
    connect(m_editObjectBtn, &QPushButton::clicked, this, &AdminMainWindow::onEditObject);
    connect(m_deleteObjectBtn, &QPushButton::clicked, this, &AdminMainWindow::onDeleteObject);
    connect(m_refreshObjectsBtn, &QPushButton::clicked, this, &AdminMainWindow::onRefreshObjects);

    // Датчики
    connect(m_addSensorBtn, &QPushButton::clicked, this, &AdminMainWindow::onAddSensor);
    connect(m_editSensorBtn, &QPushButton::clicked, this, &AdminMainWindow::onEditSensor);
    connect(m_deleteSensorBtn, &QPushButton::clicked, this, &AdminMainWindow::onDeleteSensor);
    connect(m_refreshSensorsBtn, &QPushButton::clicked, this, &AdminMainWindow::onRefreshSensors);

    // Пользователи
    connect(m_addUserBtn, &QPushButton::clicked, this, &AdminMainWindow::onAddUser);
    connect(m_editUserBtn, &QPushButton::clicked, this, &AdminMainWindow::onEditUser);
    connect(m_deleteUserBtn, &QPushButton::clicked, this, &AdminMainWindow::onDeleteUser);
    connect(m_refreshUsersBtn, &QPushButton::clicked, this, &AdminMainWindow::onRefreshUsers);

    // Двойной клик для редактирования
    connect(m_objectsTable, &QTableWidget::doubleClicked, this, &AdminMainWindow::onEditObject);
    connect(m_sensorsTable, &QTableWidget::doubleClicked, this, &AdminMainWindow::onEditSensor);
    connect(m_usersTable, &QTableWidget::doubleClicked, this, &AdminMainWindow::onEditUser);
}

void AdminMainWindow::showLoginDialog()
{
    bool ok;
    QString username = QInputDialog::getText(this, "Вход в систему администрирования",
                                            "Имя пользователя:",
                                            QLineEdit::Normal, "admin", &ok);
    if (!ok) {
        close();
        return;
    }

    QString password = QInputDialog::getText(this, "Вход в систему администрирования",
                                            "Пароль:",
                                            QLineEdit::Password, "", &ok);
    if (!ok) {
        close();
        return;
    }

    m_apiClient->login(username, password);
}

void AdminMainWindow::loadData()
{
    m_apiClient->getObjects();
    m_apiClient->getSensors();
    m_apiClient->getUsers();
}

void AdminMainWindow::onLoginSuccess(const QString &token, const QString &username)
{
    statusBar()->showMessage("Вход выполнен: " + username);
    loadData();
}

void AdminMainWindow::onLoginFailed(const QString &error)
{
    QMessageBox::critical(this, "Ошибка входа", error);
    showLoginDialog();
}

void AdminMainWindow::onLogout()
{
    m_apiClient->logout();
    showLoginDialog();
}

// ==================== ОБЪЕКТЫ ====================

void AdminMainWindow::onObjectsLoaded(const QJsonArray &objects)
{
    m_objects.clear();
    for (const QJsonValue &value : objects) {
        Object obj(value.toObject());
        m_objects.append(obj);
    }
    populateObjectsTable(m_objects);
    statusBar()->showMessage(QString("Загружено %1 объектов").arg(m_objects.size()));
}

void AdminMainWindow::onObjectCreated(const QJsonObject &object)
{
    Object obj(object);
    m_objects.append(obj);
    populateObjectsTable(m_objects);
    QMessageBox::information(this, "Успех", "Объект создан");
}

void AdminMainWindow::onObjectUpdated(const QJsonObject &object)
{
    Object updatedObj(object);
    for (int i = 0; i < m_objects.size(); ++i) {
        if (m_objects[i].objectId() == updatedObj.objectId()) {
            m_objects[i] = updatedObj;
            break;
        }
    }
    populateObjectsTable(m_objects);
    QMessageBox::information(this, "Успех", "Объект обновлён");
}

void AdminMainWindow::onObjectDeleted(const QString &objectId)
{
    for (int i = 0; i < m_objects.size(); ++i) {
        if (m_objects[i].objectId() == objectId) {
            m_objects.removeAt(i);
            break;
        }
    }
    populateObjectsTable(m_objects);
    QMessageBox::information(this, "Успех", "Объект удалён");
}

void AdminMainWindow::populateObjectsTable(const QList<Object> &objects)
{
    m_objectsTable->setRowCount(objects.size());

    for (int i = 0; i < objects.size(); ++i) {
        const Object &obj = objects[i];

        m_objectsTable->setItem(i, 0, new QTableWidgetItem(obj.objectId()));
        m_objectsTable->setItem(i, 1, new QTableWidgetItem(obj.name()));
        m_objectsTable->setItem(i, 2, new QTableWidgetItem(obj.objectType().typeName()));
        m_objectsTable->setItem(i, 3, new QTableWidgetItem(obj.status()));
        m_objectsTable->setItem(i, 4, new QTableWidgetItem(obj.parentObjectId()));
    }

    m_objectsTable->resizeColumnsToContents();
}

void AdminMainWindow::onAddObject()
{
    ObjectDialog dialog(this);
    dialog.setWindowTitle("Добавление объекта");

    // Загружаем типы объектов
    // В реальном проекте здесь должен быть вызов API для получения типов

    if (dialog.exec() == QDialog::Accepted) {
        Object obj = dialog.getObject();
        m_apiClient->createObject(obj.toJson());
    }
}

void AdminMainWindow::onEditObject()
{
    int row = m_objectsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Предупреждение", "Выберите объект для редактирования");
        return;
    }

    Object obj = m_objects[row];
    ObjectDialog dialog(this);
    dialog.setWindowTitle("Редактирование объекта");
    dialog.setObject(obj);

    if (dialog.exec() == QDialog::Accepted) {
        Object updatedObj = dialog.getObject();
        updatedObj.setObjectId(obj.objectId());
        m_apiClient->updateObject(obj.objectId(), updatedObj.toJson());
    }
}

void AdminMainWindow::onDeleteObject()
{
    int row = m_objectsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Предупреждение", "Выберите объект для удаления");
        return;
    }

    Object obj = m_objects[row];
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение",
        QString("Удалить объект '%1'?").arg(obj.name()),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_apiClient->deleteObject(obj.objectId());
    }
}

void AdminMainWindow::onRefreshObjects()
{
    m_apiClient->getObjects();
}

// ==================== ДАТЧИКИ ====================

void AdminMainWindow::onSensorsLoaded(const QJsonArray &sensors)
{
    m_sensors.clear();
    for (const QJsonValue &value : sensors) {
        Sensor sensor(value.toObject());
        m_sensors.append(sensor);
    }
    populateSensorsTable(m_sensors);
    statusBar()->showMessage(QString("Загружено %1 датчиков").arg(m_sensors.size()));
}

void AdminMainWindow::onSensorCreated(const QJsonObject &sensor)
{
    Sensor s(sensor);
    m_sensors.append(s);
    populateSensorsTable(m_sensors);
    QMessageBox::information(this, "Успех", "Датчик создан");
}

void AdminMainWindow::onSensorUpdated(const QJsonObject &sensor)
{
    Sensor updatedSensor(sensor);
    for (int i = 0; i < m_sensors.size(); ++i) {
        if (m_sensors[i].sensorId() == updatedSensor.sensorId()) {
            m_sensors[i] = updatedSensor;
            break;
        }
    }
    populateSensorsTable(m_sensors);
    QMessageBox::information(this, "Успех", "Датчик обновлён");
}

void AdminMainWindow::onSensorDeleted(const QString &sensorId)
{
    for (int i = 0; i < m_sensors.size(); ++i) {
        if (m_sensors[i].sensorId() == sensorId) {
            m_sensors.removeAt(i);
            break;
        }
    }
    populateSensorsTable(m_sensors);
    QMessageBox::information(this, "Успех", "Датчик удалён");
}

void AdminMainWindow::populateSensorsTable(const QList<Sensor> &sensors)
{
    m_sensorsTable->setRowCount(sensors.size());

    for (int i = 0; i < sensors.size(); ++i) {
        const Sensor &s = sensors[i];

        m_sensorsTable->setItem(i, 0, new QTableWidgetItem(s.sensorId()));
        m_sensorsTable->setItem(i, 1, new QTableWidgetItem(s.name()));
        m_sensorsTable->setItem(i, 2, new QTableWidgetItem(s.sensorType().typeName()));
        m_sensorsTable->setItem(i, 3, new QTableWidgetItem(s.objectId()));
        m_sensorsTable->setItem(i, 4, new QTableWidgetItem(QString::number(s.lastValue())));
        m_sensorsTable->setItem(i, 5, new QTableWidgetItem(s.status()));
    }

    m_sensorsTable->resizeColumnsToContents();
}

void AdminMainWindow::onAddSensor()
{
    SensorDialog dialog(this);
    dialog.setWindowTitle("Добавление датчика");

    if (dialog.exec() == QDialog::Accepted) {
        Sensor sensor = dialog.getSensor();
        m_apiClient->createSensor(sensor.toJson());
    }
}

void AdminMainWindow::onEditSensor()
{
    int row = m_sensorsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Предупреждение", "Выберите датчик для редактирования");
        return;
    }

    Sensor sensor = m_sensors[row];
    SensorDialog dialog(this);
    dialog.setWindowTitle("Редактирование датчика");
    dialog.setSensor(sensor);

    if (dialog.exec() == QDialog::Accepted) {
        Sensor updatedSensor = dialog.getSensor();
        updatedSensor.setSensorId(sensor.sensorId());
        m_apiClient->updateSensor(sensor.sensorId(), updatedSensor.toJson());
    }
}

void AdminMainWindow::onDeleteSensor()
{
    int row = m_sensorsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Предупреждение", "Выберите датчик для удаления");
        return;
    }

    Sensor sensor = m_sensors[row];
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение",
        QString("Удалить датчик '%1'?").arg(sensor.name()),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_apiClient->deleteSensor(sensor.sensorId());
    }
}

void AdminMainWindow::onRefreshSensors()
{
    m_apiClient->getSensors();
}

// ==================== ПОЛЬЗОВАТЕЛИ ====================

void AdminMainWindow::onUsersLoaded(const QJsonArray &users)
{
    m_users.clear();
    for (const QJsonValue &value : users) {
        User user(value.toObject());
        m_users.append(user);
    }
    populateUsersTable(m_users);
    statusBar()->showMessage(QString("Загружено %1 пользователей").arg(m_users.size()));
}

void AdminMainWindow::onUserCreated(const QJsonObject &user)
{
    User u(user);
    m_users.append(u);
    populateUsersTable(m_users);
    QMessageBox::information(this, "Успех", "Пользователь создан");
}

void AdminMainWindow::onUserUpdated(const QJsonObject &user)
{
    User updatedUser(user);
    for (int i = 0; i < m_users.size(); ++i) {
        if (m_users[i].userId() == updatedUser.userId()) {
            m_users[i] = updatedUser;
            break;
        }
    }
    populateUsersTable(m_users);
    QMessageBox::information(this, "Успех", "Пользователь обновлён");
}

void AdminMainWindow::onUserDeleted(const QString &userId)
{
    for (int i = 0; i < m_users.size(); ++i) {
        if (m_users[i].userId() == userId) {
            m_users.removeAt(i);
            break;
        }
    }
    populateUsersTable(m_users);
    QMessageBox::information(this, "Успех", "Пользователь удалён");
}

void AdminMainWindow::populateUsersTable(const QList<User> &users)
{
    m_usersTable->setRowCount(users.size());

    for (int i = 0; i < users.size(); ++i) {
        const User &u = users[i];

        m_usersTable->setItem(i, 0, new QTableWidgetItem(QString::number(u.userId())));
        m_usersTable->setItem(i, 1, new QTableWidgetItem(u.username()));
        m_usersTable->setItem(i, 2, new QTableWidgetItem(u.email()));
        m_usersTable->setItem(i, 3, new QTableWidgetItem(u.role()));
    }

    m_usersTable->resizeColumnsToContents();
}

void AdminMainWindow::onAddUser()
{
    UserDialog dialog(this);
    dialog.setWindowTitle("Добавление пользователя");

    if (dialog.exec() == QDialog::Accepted) {
        User user = dialog.getUser();
        m_apiClient->createUser(user.toJson());
    }
}

void AdminMainWindow::onEditUser()
{
    int row = m_usersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Предупреждение", "Выберите пользователя для редактирования");
        return;
    }

    User user = m_users[row];
    UserDialog dialog(this);
    dialog.setWindowTitle("Редактирование пользователя");
    dialog.setUser(user);

    if (dialog.exec() == QDialog::Accepted) {
        User updatedUser = dialog.getUser();
        updatedUser.setUserId(user.userId());
        m_apiClient->updateUser(QString::number(user.userId()), updatedUser.toJson());
    }
}

void AdminMainWindow::onDeleteUser()
{
    int row = m_usersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Предупреждение", "Выберите пользователя для удаления");
        return;
    }

    User user = m_users[row];
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение",
        QString("Удалить пользователя '%1'?").arg(user.username()),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_apiClient->deleteUser(QString::number(user.userId()));
    }
}

void AdminMainWindow::onRefreshUsers()
{
    m_apiClient->getUsers();
}