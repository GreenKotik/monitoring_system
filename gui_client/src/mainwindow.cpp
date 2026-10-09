#include "mainwindow.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_apiClient(new ApiClient(this))
    , m_appState(&AppState::instance())
{
    setupUI();
    loadObjects();

    // Подключаем сигналы
    connect(m_apiClient, &ApiClient::objectsLoaded,
            this, &MainWindow::onObjectsLoaded);

    connect(m_mapWidget, &MapWidget::objectSelected,
            this, &MainWindow::onObjectSelected);

    connect(m_toolbar, &ToolbarWidget::searchTextChanged,
            this, &MainWindow::onSearchTextChanged);
    connect(m_toolbar, &ToolbarWidget::refreshClicked,
            this, &MainWindow::onRefreshClicked);

    connect(m_breadcrumb, &BreadcrumbWidget::itemClicked,
            this, &MainWindow::onBackClicked);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    setWindowTitle("Система мониторинга объектов");
    resize(1400, 900);

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Хлебные крошки
    m_breadcrumb = new BreadcrumbWidget(this);
    m_mainLayout->addWidget(m_breadcrumb);

    // Основной контент
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // Панель инструментов
    m_toolbar = new ToolbarWidget(this);
    m_toolbar->setFixedWidth(280);
    contentLayout->addWidget(m_toolbar);

    // Стек контента
    m_contentStack = new QStackedWidget(this);
    m_mapWidget = new MapWidget(this);
    m_objectDetailWidget = new ObjectDetailWidget(this);
    m_sensorDetailWidget = new SensorDetailWidget(this);

    m_contentStack->addWidget(m_mapWidget);
    m_contentStack->addWidget(m_objectDetailWidget);
    m_contentStack->addWidget(m_sensorDetailWidget);


    contentLayout->addWidget(m_contentStack, 1);

    m_mainLayout->addLayout(contentLayout);

    statusBar()->showMessage("Готов к работе");
}

void MainWindow::loadObjects() {
    m_apiClient->getObjects();
}

// ДОБАВЛЕНО: реализация слота
void MainWindow::onObjectsLoaded(const QList<Object> &objects) {
    m_objects = objects;
    m_mapWidget->setObjects(objects);
    m_breadcrumb->clear();
    m_breadcrumb->addItem("Карта страны", "country");
    showLevel(0);
    statusBar()->showMessage(QString("Загружено %1 объектов").arg(objects.size()));
}

void MainWindow::onObjectSelected(const QString &objectId) {
    Object obj;
    for (const auto &o : m_objects) {
        if (o.id() == objectId) {
            obj = o;
            break;
        }
    }

    if (obj.isValid()) {
        m_appState->setCurrentObject(obj);
        m_breadcrumb->addItem(obj.name(), objectId);
        m_objectDetailWidget->setObject(obj);
        showLevel(1);
        statusBar()->showMessage("Выбран объект: " + obj.name());
    }
}

void MainWindow::onSensorSelected(const QString &sensorId) {
    Sensor sensor;
    for (const auto &s : m_sensors) {
        if (s.id() == sensorId) {
            sensor = s;
            break;
        }
    }

    if (sensor.isValid()) {
        m_appState->setCurrentSensor(sensor);
        m_breadcrumb->addItem(sensor.name(), sensorId);
        m_sensorDetailWidget->setSensor(sensor);
        showLevel(2);
        statusBar()->showMessage("Выбран датчик: " + sensor.name());
    }
}

void MainWindow::onBackClicked() {
    m_breadcrumb->popItem();

    if (m_breadcrumb->count() <= 1) {
        showLevel(0);
        statusBar()->showMessage("Возврат на карту");
    } else {
        showLevel(1);
        QString lastId = m_breadcrumb->lastId();
        if (!lastId.isEmpty()) {
            onObjectSelected(lastId);
        }
    }
}

void MainWindow::onSearchTextChanged(const QString &text) {
    m_mapWidget->filterObjects(text);
}

void MainWindow::onRefreshClicked() {
    loadObjects();
    statusBar()->showMessage("Данные обновлены");
}

void MainWindow::showLevel(int level) {
    m_contentStack->setCurrentIndex(level);
    m_toolbar->setLevel(level);
}
