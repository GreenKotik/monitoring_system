#include "dialogs/sensordialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>

SensorDialog::SensorDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadSensorTypes();
}

void SensorDialog::setupUi()
{
    setWindowTitle("Датчик");
    setMinimumWidth(400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();

    m_idEdit = new QLineEdit(this);
    formLayout->addRow("ID:", m_idEdit);

    m_nameEdit = new QLineEdit(this);
    formLayout->addRow("Название:", m_nameEdit);

    m_descriptionEdit = new QLineEdit(this);
    formLayout->addRow("Описание:", m_descriptionEdit);

    m_typeCombo = new QComboBox(this);
    formLayout->addRow("Тип:", m_typeCombo);

    m_objectIdEdit = new QLineEdit(this);
    formLayout->addRow("Объект ID:", m_objectIdEdit);

    m_positionX = new QDoubleSpinBox(this);
    m_positionX->setRange(-10000, 10000);
    m_positionX->setDecimals(2);
    m_positionX->setValue(50);
    formLayout->addRow("Позиция X (%):", m_positionX);

    m_positionY = new QDoubleSpinBox(this);
    m_positionY->setRange(-10000, 10000);
    m_positionY->setDecimals(2);
    m_positionY->setValue(50);
    formLayout->addRow("Позиция Y (%):", m_positionY);

    m_statusCombo = new QComboBox(this);
    m_statusCombo->addItems({"active", "inactive", "error"});
    formLayout->addRow("Статус:", m_statusCombo);

    m_lastValue = new QDoubleSpinBox(this);
    m_lastValue->setRange(-1000000, 1000000);
    m_lastValue->setDecimals(3);
    formLayout->addRow("Последнее значение:", m_lastValue);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okBtn = new QPushButton("OK", this);
    QPushButton *cancelBtn = new QPushButton("Отмена", this);

    okBtn->setProperty("acceptButton", true);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okBtn);
    buttonLayout->addWidget(cancelBtn);

    mainLayout->addLayout(buttonLayout);
}

void SensorDialog::loadSensorTypes()
{
    m_typeCombo->addItem("Температура", 1);
    m_typeCombo->addItem("Влажность", 2);
    m_typeCombo->addItem("CO2", 3);
}

void SensorDialog::setSensor(const Sensor &sensor)
{
    m_sensor = sensor;

    m_idEdit->setText(sensor.sensorId());
    m_nameEdit->setText(sensor.name());
    m_descriptionEdit->setText(sensor.description());

    int index = m_typeCombo->findData(sensor.typeId());
    if (index >= 0) {
        m_typeCombo->setCurrentIndex(index);
    }

    m_objectIdEdit->setText(sensor.objectId());
    m_positionX->setValue(sensor.positionX());
    m_positionY->setValue(sensor.positionY());

    int statusIndex = m_statusCombo->findText(sensor.status());
    if (statusIndex >= 0) {
        m_statusCombo->setCurrentIndex(statusIndex);
    }

    m_lastValue->setValue(sensor.lastValue());
}

Sensor SensorDialog::getSensor() const
{
    Sensor sensor;

    sensor.setSensorId(m_idEdit->text().trimmed());
    sensor.setName(m_nameEdit->text().trimmed());
    sensor.setDescription(m_descriptionEdit->text().trimmed());
    sensor.setTypeId(m_typeCombo->currentData().toInt());
    sensor.setObjectId(m_objectIdEdit->text().trimmed());
    sensor.setPositionX(m_positionX->value());
    sensor.setPositionY(m_positionY->value());
    sensor.setStatus(m_statusCombo->currentText());
    sensor.setLastValue(m_lastValue->value());

    return sensor;
}