#include "dialogs/objectdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

ObjectDialog::ObjectDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadObjectTypes();
}

void ObjectDialog::setupUi()
{
    setWindowTitle("Объект");
    setMinimumWidth(450);

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

    m_parentIdEdit = new QLineEdit(this);
    formLayout->addRow("Родительский ID:", m_parentIdEdit);

    m_positionX = new QDoubleSpinBox(this);
    m_positionX->setRange(-10000, 10000);
    m_positionX->setDecimals(2);
    formLayout->addRow("Позиция X:", m_positionX);

    m_positionY = new QDoubleSpinBox(this);
    m_positionY->setRange(-10000, 10000);
    m_positionY->setDecimals(2);
    formLayout->addRow("Позиция Y:", m_positionY);

    m_sizeWidth = new QDoubleSpinBox(this);
    m_sizeWidth->setRange(0, 10000);
    m_sizeWidth->setDecimals(2);
    m_sizeWidth->setValue(100);
    formLayout->addRow("Ширина:", m_sizeWidth);

    m_sizeHeight = new QDoubleSpinBox(this);
    m_sizeHeight->setRange(0, 10000);
    m_sizeHeight->setDecimals(2);
    m_sizeHeight->setValue(80);
    formLayout->addRow("Высота:", m_sizeHeight);

    m_svgPathEdit = new QLineEdit(this);
    formLayout->addRow("Путь к SVG:", m_svgPathEdit);

    m_statusCombo = new QComboBox(this);
    m_statusCombo->addItems({"active", "inactive", "maintenance"});
    formLayout->addRow("Статус:", m_statusCombo);

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

void ObjectDialog::loadObjectTypes()
{
    m_typeCombo->addItem("Комплекс", 1);
    m_typeCombo->addItem("Здание", 2);
    m_typeCombo->addItem("Помещение", 3);
}

void ObjectDialog::setObject(const Object &object)
{
    m_object = object;

    m_idEdit->setText(object.objectId());
    m_nameEdit->setText(object.name());
    m_descriptionEdit->setText(object.description());

    int index = m_typeCombo->findData(object.objectTypeId());
    if (index >= 0) {
        m_typeCombo->setCurrentIndex(index);
    }

    m_parentIdEdit->setText(object.parentObjectId());
    m_positionX->setValue(object.positionX());
    m_positionY->setValue(object.positionY());
    m_sizeWidth->setValue(object.sizeWidth());
    m_sizeHeight->setValue(object.sizeHeight());
    m_svgPathEdit->setText(object.svgSchemePath());

    int statusIndex = m_statusCombo->findText(object.status());
    if (statusIndex >= 0) {
        m_statusCombo->setCurrentIndex(statusIndex);
    }
}

Object ObjectDialog::getObject() const
{
    Object obj;

    obj.setObjectId(m_idEdit->text().trimmed());
    obj.setName(m_nameEdit->text().trimmed());
    obj.setDescription(m_descriptionEdit->text().trimmed());
    obj.setObjectTypeId(m_typeCombo->currentData().toInt());
    obj.setParentObjectId(m_parentIdEdit->text().trimmed());
    obj.setPositionX(m_positionX->value());
    obj.setPositionY(m_positionY->value());
    obj.setSizeWidth(m_sizeWidth->value());
    obj.setSizeHeight(m_sizeHeight->value());
    obj.setSvgSchemePath(m_svgPathEdit->text().trimmed());
    obj.setStatus(m_statusCombo->currentText());

    return obj;
}