#include "widgets/sensordetailwidget.h"
#include <QGridLayout>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>

SensorDetailWidget::SensorDetailWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentRange("day")
{
    setupUI();
}

SensorDetailWidget::~SensorDetailWidget() {}

void SensorDetailWidget::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    // Группа информации о датчике
    m_infoGroup = new QGroupBox("Информация о датчике", this);
    m_infoLayout = new QGridLayout(m_infoGroup);
    m_infoLayout->setSpacing(5);

    int row = 0;
    m_infoLayout->addWidget(new QLabel("Название:"), row, 0);
    m_nameLabel = new QLabel("-", this);
    m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
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
    m_infoLayout->addWidget(new QLabel("Расположение:"), row, 0);
    m_locationLabel = new QLabel("-", this);
    m_infoLayout->addWidget(m_locationLabel, row, 1);

    row++;
    m_infoLayout->addWidget(new QLabel("Единица измерения:"), row, 0);
    m_unitLabel = new QLabel("-", this);
    m_infoLayout->addWidget(m_unitLabel, row, 1);

    // Текущее значение
    row++;
    m_infoLayout->addWidget(new QLabel("Текущее значение:"), row, 0);
    m_currentValueLabel = new QLabel("-", this);
    m_currentValueLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #0066cc;");
    m_infoLayout->addWidget(m_currentValueLabel, row, 1);

    row++;
    m_infoLayout->addWidget(new QLabel("Статус:"), row, 0);
    m_statusLabel = new QLabel("-", this);
    m_statusLabel->setStyleSheet("font-weight: bold;");
    m_infoLayout->addWidget(m_statusLabel, row, 1);

    row++;
    m_infoLayout->addWidget(new QLabel("Последнее обновление:"), row, 0);
    m_lastUpdateLabel = new QLabel("-", this);
    m_infoLayout->addWidget(m_lastUpdateLabel, row, 1);

    m_mainLayout->addWidget(m_infoGroup);

    // График (заменен на самописный)
    m_chartWidget = new SensorChartWidget(this);
    m_chartWidget->setMinimumHeight(200);
    m_mainLayout->addWidget(m_chartWidget, 1);

    // Таблица показаний
    m_readingsGroup = new QGroupBox("Последние показания", this);
    QVBoxLayout *readingsLayout = new QVBoxLayout(m_readingsGroup);
    m_readingsTable = new QTableWidget(this);
    m_readingsTable->setColumnCount(2);
    m_readingsTable->setHorizontalHeaderLabels({"Время", "Значение"});
    m_readingsTable->horizontalHeader()->setStretchLastSection(true);
    m_readingsTable->verticalHeader()->setVisible(false);
    m_readingsTable->setAlternatingRowColors(true);
    readingsLayout->addWidget(m_readingsTable);
    m_mainLayout->addWidget(m_readingsGroup);

    // Управление
    m_controlsLayout = new QHBoxLayout();
    m_refreshBtn = new QPushButton("🔄 Обновить", this);
    connect(m_refreshBtn, &QPushButton::clicked, this, &SensorDetailWidget::onRefreshClicked);
    m_controlsLayout->addWidget(m_refreshBtn);

    m_controlsLayout->addStretch();

    m_timeRangeCombo = new QComboBox(this);
    m_timeRangeCombo->addItems({"Час", "День", "Неделя", "Месяц"});
    connect(m_timeRangeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SensorDetailWidget::onTimeRangeChanged);
    m_controlsLayout->addWidget(new QLabel("Период:"));
    m_controlsLayout->addWidget(m_timeRangeCombo);

    m_mainLayout->addLayout(m_controlsLayout);
}

void SensorDetailWidget::setSensor(const Sensor &sensor) {
    m_currentSensor = sensor;
    updateInfo();
}

void SensorDetailWidget::setReadings(const QList<SensorReading> &readings) {
    m_readings = readings;
    updateReadings();
    updateChart();
}

void SensorDetailWidget::clear() {
    m_currentSensor = Sensor();
    m_readings.clear();
    
    m_nameLabel->setText("-");
    m_typeLabel->setText("-");
    m_idLabel->setText("-");
    m_locationLabel->setText("-");
    m_unitLabel->setText("-");
    m_currentValueLabel->setText("-");
    m_statusLabel->setText("-");
    m_lastUpdateLabel->setText("-");
    m_readingsTable->setRowCount(0);
    m_chartWidget->clear();
}

void SensorDetailWidget::updateInfo() {
    if (!m_currentSensor.isValid()) {
        clear();
        return;
    }

    m_nameLabel->setText(m_currentSensor.name());
    m_typeLabel->setText(m_currentSensor.typeName());
    m_idLabel->setText(m_currentSensor.id());
    m_locationLabel->setText(m_currentSensor.objectName());
    m_unitLabel->setText(m_currentSensor.unit());
    
    // Текущее значение
    double value = m_currentSensor.lastValue();
    QString valueText = QString::number(value, 'f', 2);
    m_currentValueLabel->setText(valueText + " " + m_currentSensor.unit());
    
    // Статус
    QString status = m_currentSensor.status();
    if (status == "normal") {
        m_statusLabel->setText("🟢 Норма");
        m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
    } else if (status == "warning") {
        m_statusLabel->setText("🟡 Предупреждение");
        m_statusLabel->setStyleSheet("color: orange; font-weight: bold;");
    } else if (status == "error") {
        m_statusLabel->setText("🔴 Ошибка");
        m_statusLabel->setStyleSheet("color: red; font-weight: bold;");
    } else {
        m_statusLabel->setText("⚪ Неизвестно");
        m_statusLabel->setStyleSheet("color: gray; font-weight: bold;");
    }
    
    // Время обновления
    m_lastUpdateLabel->setText(m_currentSensor.lastUpdate().toString("dd.MM.yyyy HH:mm:ss"));
}

void SensorDetailWidget::updateReadings() {
    m_readingsTable->setRowCount(m_readings.size());
    
    for (int i = 0; i < m_readings.size(); ++i) {
        const SensorReading &reading = m_readings[i];
        
        QTableWidgetItem *timeItem = new QTableWidgetItem(
            reading.timestamp().toString("dd.MM.yyyy HH:mm:ss")
        );
        m_readingsTable->setItem(i, 0, timeItem);
        
        QTableWidgetItem *valueItem = new QTableWidgetItem(
            QString::number(reading.value(), 'f', 2) + " " + m_currentSensor.unit()
        );
        m_readingsTable->setItem(i, 1, valueItem);
    }
    
    m_readingsTable->resizeColumnsToContents();
}

void SensorDetailWidget::updateChart() {
    m_chartWidget->setData(m_readings, m_currentSensor.unit());
}

void SensorDetailWidget::onRefreshClicked() {
    emit refreshRequested();
}

void SensorDetailWidget::onTimeRangeChanged(int index) {
    QStringList ranges = {"hour", "day", "week", "month"};
    if (index >= 0 && index < ranges.size()) {
        m_currentRange = ranges[index];
        emit refreshRequested();
    }
}