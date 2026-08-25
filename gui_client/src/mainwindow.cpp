#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_apiClient = new ApiClient(this);
    m_apiClient->setBaseUrl("http://localhost:3000/api");

    setupUi();
    setupConnections();

    showLoginDialog();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    setWindowTitle("Система мониторинга");
    setMinimumSize(1200, 800);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Хлебные крошки
    m_breadcrumbWidget = new BreadcrumbWidget(this);
    mainLayout->addWidget(m_breadcrumbWidget);

    // Основной сплиттер
    m_splitter = new QSplitter(Qt::Horizontal, this);

    // Панель инструментов
    m_toolbarWidget = new ToolbarWidget(this);
    m_splitter->addWidget(m_toolbarWidget);

    // Контентная область
    QWidget *contentWidget = new QWidget(this);
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    m_stackedWidget = new QStackedWidget(this);

    // Уровень карты
    m_mapWidget = new MapWidget(this);
    m_mapWidget->loadMap(":/maps/russia.svg");
    m_stackedWidget->addWidget(m_mapWidget);

    // Уровень объекта (схема)
    QWidget *objectWidget = new QWidget(this);
    QVBoxLayout *objectLayout = new QVBoxLayout(objectWidget);
    objectLayout->setContentsMargins(0, 0, 0, 0);

    // Здесь будет схема объекта
    QLabel *objectLabel = new QLabel("Схема объекта", objectWidget);
    objectLabel->setAlignment(Qt::AlignCenter);
    objectLayout->addWidget(objectLabel);
    m_stackedWidget->addWidget(objectWidget);

    // Уровень датчика
    m_sensorDetailWidget = new SensorDetailWidget(this);
    m_stackedWidget->addWidget(m_sensorDetailWidget);

    contentLayout->addWidget(m_stackedWidget);
    m_splitter->addWidget(contentWidget);

    m_splitter->setSizes({280, 800});
    mainLayout->addWidget(m_splitter);

    // Статус бар
    statusBar()->showMessage("Готов к работе");
}

void MainWindow::setupConnections()
{
    connect(m_apiClient, &ApiClient::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(m_apiClient, &ApiClient::loginFailed, this, &MainWindow::onLoginFailed);
    connect(m_apiClient, &ApiClient::objectsReceived, this, &MainWindow::onObjectsLoaded);
    connect(m_apiClient, &ApiClient::sensorsReceived, this, &MainWindow::onSensorsLoaded);
    connect(m_apiClient, &ApiClient::sensorHistoryReceived, this, &MainWindow::onHistoryLoaded);
    connect(m_apiClient, &ApiClient::errorOccurred, this, &MainWindow::onApiError);

    connect(m_toolbarWidget, &ToolbarWidget::objectSelected, this, &MainWindow::onObjectSelected);
    connect(m_toolbarWidget, &ToolbarWidget::sensorSelected, this, &MainWindow::onSensorSelected);
    connect(m_toolbarWidget, &ToolbarWidget::backClicked, this, &MainWindow::onBackClicked);
    connect(m_toolbarWidget, &ToolbarWidget::viewReset, this, &MainWindow::onViewReset);
    connect(m_toolbarWidget, &ToolbarWidget::zoomIn, this, &MainWindow::onZoomIn);
    connect(m_toolbarWidget, &ToolbarWidget::zoomOut, this, &MainWindow::onZoomOut);

    connect(m_breadcrumbWidget, &BreadcrumbWidget::itemClicked, this, &MainWindow::onObjectSelected);

    connect(m_mapWidget, &MapWidget::objectClicked, this, &MainWindow::onObjectSelected);
    connect(m_mapWidget, &MapWidget::sensorClicked, this, &MainWindow::onSensorSelected);

    connect(m_sensorDetailWidget, &SensorDetailWidget::backClicked, this, &MainWindow::onBackClicked);
}

void MainWindow::showLoginDialog()
{
    bool ok;
    QString username = QInputDialog::getText(this, "Вход в систему",
                                            "Имя пользователя:",
                                            QLineEdit::Normal, "admin", &ok);
    if (!ok) {
        close();
        return;
    }

    QString password = QInputDialog::getText(this, "Вход в систему",
                                            "Пароль:",
                                            QLineEdit::Password, "", &ok);
    if (!ok) {
        close();
        return;
    }

    m_apiClient->login(username, password);
}

void MainWindow::onLoginSuccess(const QString &token, const QString &username)
{
    statusBar()->showMessage("Добро пожаловать, " + username);
    loadInitialData();
}

void MainWindow::onLoginFailed(const QString &error)
{
    QMessageBox::critical(this, "Ошибка входа", error);
    showLoginDialog();
}

void MainWindow::loadInitialData()
{
    m_apiClient->getObjects(true);
}

void MainWindow::onObjectsLoaded(const QJsonArray &objects)
{
    m_toolbarWidget->setObjects(objects);
    m_mapWidget->setObjects(objects);
    statusBar()->showMessage(QString("Загружено %1 объектов").arg(objects.size()));
}

void MainWindow::onSensorsLoaded(const QJsonArray &sensors)
{
    m_toolbarWidget->setSensors(sensors);
    m_mapWidget->setSensors(sensors);
    statusBar()->showMessage(QString("Загружено %1 датчиков").arg(sensors.size()));
}

void MainWindow::onHistoryLoaded(const QJsonArray &history)
{
    m_sensorDetailWidget->setHistory(history);
}

void MainWindow::onObjectSelected(const QString &objectId)
{
    Logger::instance().info("Object selected: " + objectId);
    m_appState.setCurrentObject(objectId);
    m_appState.setLevel(AppState::LevelObject);

    m_apiClient->getObject(objectId);
    m_apiClient->getSensors(objectId);

    m_stackedWidget->setCurrentIndex(1);
    updateToolbar();
    updateBreadcrumb();
}

void MainWindow::onSensorSelected(const QString &sensorId)
{
    Logger::instance().info("Sensor selected: " + sensorId);
    m_appState.setCurrentSensor(sensorId);
    m_appState.setLevel(AppState::LevelSensor);

    m_apiClient->getSensor(sensorId);
    m_apiClient->getSensorHistory(sensorId, 100);

    m_stackedWidget->setCurrentIndex(2);
    updateToolbar();
    updateBreadcrumb();
}

void MainWindow::onBackClicked()
{
    if (m_appState.level() == AppState::LevelSensor) {
        m_appState.setLevel(AppState::LevelObject);
        m_appState.setCurrentSensor(QString());
        m_stackedWidget->setCurrentIndex(1);
    } else if (m_appState.level() == AppState::LevelObject) {
        m_appState.setLevel(AppState::LevelCountry);
        m_appState.setCurrentObject(QString());
        m_stackedWidget->setCurrentIndex(0);
    }

    updateToolbar();
    updateBreadcrumb();
}

void MainWindow::onViewReset()
{
    m_mapWidget->resetView();
}

void MainWindow::onZoomIn()
{
    m_mapWidget->zoomIn();
}

void MainWindow::onZoomOut()
{
    m_mapWidget->zoomOut();
}

void MainWindow::onApiError(const QString &error)
{
    statusBar()->showMessage("Ошибка: " + error);
}

void MainWindow::updateBreadcrumb()
{
    m_breadcrumbWidget->setLevel(m_appState.level());
    m_breadcrumbWidget->setObjectId(m_appState.currentObject());
    m_breadcrumbWidget->setSensorId(m_appState.currentSensor());
}

void MainWindow::updateToolbar()
{
    m_toolbarWidget->setLevel(m_appState.level());
}

void MainWindow::setLevel(int level)
{
    m_appState.setLevel(level);
    updateToolbar();
    updateBreadcrumb();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Logger::instance().info("Application closed");
    event->accept();
}