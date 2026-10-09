#include "widgets/objectdetailwidget.h"
#include <QGridLayout>
#include <QHeaderView>
#include <QDebug>

ObjectDetailWidget::ObjectDetailWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

ObjectDetailWidget::~ObjectDetailWidget() {}

void ObjectDetailWidget::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    // Группа информации об объекте
    m_infoGroup = new QGroupBox("Информация об объекте", this);
    m_infoLayout = new QGridLayout(m_infoGroup);
    m_infoLayout->setSpacing(5);

    int row = 0;
    m_infoLayout->addWidget(new QLabel("Название:"), row, 0);
    m_nameLabel = new QLabel("-", this);
    m_nameLabel->setStyleSheet("font-weight: bold;");
    m_infoLayout->addWidget(m_nameLabel, row, 1);

    row++;
    m_infoLayout->addWidget(new QLabel("Тип:"), row, 0);
    m_typeLabel = new QLabel("-", this);
    m_infoLayout->addWidget(m_typeLabel, row, 1);

    row++;
    m_infoLayout->addWidget(new QLabel("ID:"), row, 0);
    m_idLabel = new QLabel("-", this);
    m_infoLayout->addWidget(m_idLabel, row, 1);

    row++;
    m_infoLayout->addWidget(new QLabel("Родитель:"), row, 0);
    m_parentLabel = new QLabel("-", this);
    m_infoLayout->addWidget(m_parentLabel, row, 1);

    row++;
    m_infoLayout->addWidget(new QLabel("Статус:"), row, 0);
    m_statusLabel = new QLabel("-", this);
    m_infoLayout->addWidget(m_statusLabel, row, 1);

    m_mainLayout->addWidget(m_infoGroup);

    // Дочерние объекты
    m_childrenGroup = new QGroupBox("Дочерние объекты", this);
    QVBoxLayout *childrenLayout = new QVBoxLayout(m_childrenGroup);
    m_childrenList = new QListWidget(this);
    connect(m_childrenList, &QListWidget::itemClicked,
            this, &ObjectDetailWidget::onChildObjectClicked);
    childrenLayout->addWidget(m_childrenList);
    m_mainLayout->addWidget(m_childrenGroup);

    // Датчики
    m_sensorsGroup = new QGroupBox("Датчики", this);
    QVBoxLayout *sensorsLayout = new QVBoxLayout(m_sensorsGroup);
    m_sensorsList = new QListWidget(this);
    connect(m_sensorsList, &QListWidget::itemClicked,
            this, &ObjectDetailWidget::onSensorClicked);
    sensorsLayout->addWidget(m_sensorsList);
    m_mainLayout->addWidget(m_sensorsGroup);

    // Статистика
    m_statsLabel = new QLabel("Объектов: 0 | Датчиков: 0", this);
    m_statsLabel->setStyleSheet("color: #666; font-size: 11px;");
    m_mainLayout->addWidget(m_statsLabel);

    // Растяжка
    m_mainLayout->addStretch();
}

void ObjectDetailWidget::setObject(const Object &object) {
    m_currentObject = object;
    updateInfo();
}

void ObjectDetailWidget::setChildObjects(const QList<Object> &children) {
    m_children = children;
    updateChildren();
}

void ObjectDetailWidget::setSensors(const QList<Sensor> &sensors) {
    m_sensors = sensors;
    updateSensors();
}

void ObjectDetailWidget::clear() {
    m_currentObject = Object();
    m_children.clear();
    m_sensors.clear();

    m_nameLabel->setText("-");
    m_typeLabel->setText("-");
    m_idLabel->setText("-");
    m_parentLabel->setText("-");
    m_statusLabel->setText("-");
    m_childrenList->clear();
    m_sensorsList->clear();
    m_statsLabel->setText("Объектов: 0 | Датчиков: 0");
}

void ObjectDetailWidget::updateInfo() {
    if (!m_currentObject.isValid()) {
        clear();
        return;
    }

    m_nameLabel->setText(m_currentObject.name());
    m_typeLabel->setText(m_currentObject.typeName());
    m_idLabel->setText(m_currentObject.id());
    m_parentLabel->setText(m_currentObject.parentObjectId().isEmpty() ?
                           "Корневой" : m_currentObject.parentObjectId());

    QString status = "🟢 Норма";
    m_statusLabel->setText(status);
    m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
}

void ObjectDetailWidget::updateChildren() {
    m_childrenList->clear();

    if (m_children.isEmpty()) {
        m_childrenList->addItem("Нет дочерних объектов");
        m_childrenList->item(0)->setFlags(Qt::NoItemFlags);
        return;
    }

    for (const Object &child : m_children) {
        QString displayText = QString("%1 (%2)")
            .arg(child.name())
            .arg(child.typeName());

        QListWidgetItem *item = new QListWidgetItem(displayText, m_childrenList);
        item->setData(Qt::UserRole, child.id());
    }

    updateStats();
}

void ObjectDetailWidget::updateSensors() {
    m_sensorsList->clear();

    if (m_sensors.isEmpty()) {
        m_sensorsList->addItem("Нет датчиков");
        m_sensorsList->item(0)->setFlags(Qt::NoItemFlags);
        return;
    }

    for (const Sensor &sensor : m_sensors) {
        QString displayText = QString("%1: %2 %3")
            .arg(sensor.name())
            .arg(sensor.lastValue())
            .arg(sensor.unit());

        QListWidgetItem *item = new QListWidgetItem(displayText, m_sensorsList);
        item->setData(Qt::UserRole, sensor.id());

        if (sensor.status() == "warning") {
            item->setBackground(QColor(255, 255, 200));
        } else if (sensor.status() == "error") {
            item->setBackground(QColor(255, 200, 200));
        }
    }

    updateStats();
}

// ДОБАВЛЕНО: реализация updateStats
void ObjectDetailWidget::updateStats() {
    m_statsLabel->setText(QString("Объектов: %1 | Датчиков: %2")
        .arg(m_children.size())
        .arg(m_sensors.size()));
}

void ObjectDetailWidget::onChildObjectClicked(QListWidgetItem *item) {
    QString objectId = item->data(Qt::UserRole).toString();
    if (!objectId.isEmpty()) {
        emit childObjectSelected(objectId);
    }
}

void ObjectDetailWidget::onSensorClicked(QListWidgetItem *item) {
    QString sensorId = item->data(Qt::UserRole).toString();
    if (!sensorId.isEmpty()) {
        emit sensorSelected(sensorId);
    }
}
