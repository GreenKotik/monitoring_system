#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QGroupBox>
#include <QTableWidget>
#include <QGridLayout>
#include "models/object.h"
#include "models/sensor.h"

class ObjectDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectDetailWidget(QWidget *parent = nullptr);
    ~ObjectDetailWidget();

    void setObject(const Object &object);
    void setChildObjects(const QList<Object> &children);
    void setSensors(const QList<Sensor> &sensors);
    void clear();

signals:
    void childObjectSelected(const QString &objectId);
    void sensorSelected(const QString &sensorId);

private slots:
    void onChildObjectClicked(QListWidgetItem *item);
    void onSensorClicked(QListWidgetItem *item);

private:
    void setupUI();
    void updateInfo();
    void updateChildren();
    void updateSensors();
    void updateStats();  // ДОБАВЛЕНО: объявление метода

    Object m_currentObject;
    QList<Object> m_children;
    QList<Sensor> m_sensors;

    // UI элементы
    QVBoxLayout *m_mainLayout;

    QGroupBox *m_infoGroup;
    QGridLayout *m_infoLayout;
    QLabel *m_nameLabel;
    QLabel *m_typeLabel;
    QLabel *m_idLabel;
    QLabel *m_parentLabel;
    QLabel *m_statusLabel;

    QGroupBox *m_childrenGroup;
    QListWidget *m_childrenList;

    QGroupBox *m_sensorsGroup;
    QListWidget *m_sensorsList;

    QLabel *m_statsLabel;
};
