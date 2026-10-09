#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>      // ДОБАВЛЕНО
#include <QGridLayout>    // ДОБАВЛЕНО
#include <QHeaderView>    // ДОБАВЛЕНО
#include "models/sensor.h"
#include "models/sensorreading.h"
#include "widgets/sensorchartwidget.h"

class SensorDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SensorDetailWidget(QWidget *parent = nullptr);
    ~SensorDetailWidget();

    void setSensor(const Sensor &sensor);
    void setReadings(const QList<SensorReading> &readings);
    void clear();

signals:
    void backRequested();
    void refreshRequested();

private slots:
    void onRefreshClicked();
    void onTimeRangeChanged(int index);

private:
    void setupUI();
    void updateInfo();
    void updateReadings();
    void updateChart();

    Sensor m_currentSensor;
    QList<SensorReading> m_readings;
    QString m_currentRange;

    // UI элементы
    QVBoxLayout *m_mainLayout;

    QGroupBox *m_infoGroup;
    QGridLayout *m_infoLayout;
    QLabel *m_nameLabel;
    QLabel *m_typeLabel;
    QLabel *m_idLabel;
    QLabel *m_locationLabel;
    QLabel *m_unitLabel;
    QLabel *m_currentValueLabel;
    QLabel *m_statusLabel;
    QLabel *m_lastUpdateLabel;

    SensorChartWidget *m_chartWidget;

    QGroupBox *m_readingsGroup;
    QTableWidget *m_readingsTable;

    QHBoxLayout *m_controlsLayout;
    QPushButton *m_refreshBtn;
    QComboBox *m_timeRangeCombo;
};
