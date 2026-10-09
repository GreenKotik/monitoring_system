#ifndef SENSORDIALOG_H
#define SENSORDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include "models/sensor.h"  // Вместо "core/models/sensor.h"

class SensorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SensorDialog(QWidget *parent = nullptr);

    void setSensor(const Sensor &sensor);
    Sensor getSensor() const;

private:
    void setupUi();
    void loadSensorTypes();

    QLineEdit *m_idEdit;
    QLineEdit *m_nameEdit;
    QLineEdit *m_descriptionEdit;
    QComboBox *m_typeCombo;
    QLineEdit *m_objectIdEdit;
    QDoubleSpinBox *m_positionX;
    QDoubleSpinBox *m_positionY;
    QComboBox *m_statusCombo;
    QDoubleSpinBox *m_lastValue;

    Sensor m_sensor;
};

#endif // SENSORDIALOG_H
