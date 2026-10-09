#include "widgets/sensorchartwidget.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDateTime>
#include <QDebug>
#include <QtMath>

SensorChartWidget::SensorChartWidget(QWidget *parent)
    : QWidget(parent)
    , m_hasData(false)
    , m_zoomLevel(1.0)
    , m_offsetX(0)
    , m_showTooltip(false)
{
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    setStyleSheet(QString(
        "background-color: white; "
        "border: 1px solid #ddd; "
        "border-radius: 4px;"
    ));
}

SensorChartWidget::~SensorChartWidget() {}

void SensorChartWidget::setData(const QList<SensorReading> &readings, const QString &unit) {
    m_readings = readings;
    m_unit = unit;
    m_hasData = !readings.isEmpty();
    m_zoomLevel = 1.0;
    m_offsetX = 0;
    updateChart();
}

void SensorChartWidget::clear() {
    m_readings.clear();
    m_hasData = false;
    m_zoomLevel = 1.0;
    m_offsetX = 0;
    m_showTooltip = false;
    updateChart();
}

void SensorChartWidget::setLineColor(const QColor &color) {
    m_lineColor = color;
    updateChart();
}

void SensorChartWidget::setBackgroundColor(const QColor &color) {
    m_backgroundColor = color;
    updateChart();
}

void SensorChartWidget::updateChart() {
    update(); // Вызываем перерисовку
}

void SensorChartWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Отрисовка фона
    painter.fillRect(rect(), m_backgroundColor);
    
    if (!m_hasData || m_readings.isEmpty()) {
        painter.setPen(QPen(Qt::gray, 1));
        painter.drawText(rect(), Qt::AlignCenter, "Нет данных для отображения");
        return;
    }
    
    // Отступы для осей и подписей
    const int leftMargin = 50;
    const int rightMargin = 20;
    const int topMargin = 30;
    const int bottomMargin = 35;
    
    QRect chartRect = rect().adjusted(
        leftMargin, 
        topMargin, 
        -rightMargin, 
        -bottomMargin
    );
    
    if (chartRect.width() < 10 || chartRect.height() < 10) return;
    
    // Определяем диапазоны данных с учетом зума
    qint64 minTime = m_readings.first().timestamp().toMSecsSinceEpoch();
    qint64 maxTime = m_readings.last().timestamp().toMSecsSinceEpoch();
    qint64 timeRange = maxTime - minTime;
    
    // Применяем зум и смещение
    qint64 visibleRange = timeRange / m_zoomLevel;
    qint64 centerTime = (minTime + maxTime) / 2 + m_offsetX;
    qint64 visibleMinTime = centerTime - visibleRange / 2;
    qint64 visibleMaxTime = centerTime + visibleRange / 2;
    
    // Фильтруем данные для отображения
    QList<SensorReading> visibleReadings;
    for (const SensorReading &reading : m_readings) {
        qint64 time = reading.timestamp().toMSecsSinceEpoch();
        if (time >= visibleMinTime && time <= visibleMaxTime) {
            visibleReadings.append(reading);
        }
    }
    
    if (visibleReadings.isEmpty()) {
        painter.setPen(QPen(Qt::gray, 1));
        painter.drawText(rect(), Qt::AlignCenter, "Нет данных в выбранном диапазоне");
        return;
    }
    
    // Вычисляем min/max значений
    double minValue = visibleReadings.first().value();
    double maxValue = visibleReadings.first().value();
    
    for (const SensorReading &reading : visibleReadings) {
        double val = reading.value();
        if (val < minValue) minValue = val;
        if (val > maxValue) maxValue = val;
    }
    
    // Добавляем отступы для значений
    double valueRange = maxValue - minValue;
    if (valueRange < 0.001) {
        minValue -= 1.0;
        maxValue += 1.0;
    } else {
        double padding = valueRange * 0.1;
        minValue -= padding;
        maxValue += padding;
        if (minValue < 0 && padding > 0) minValue = 0;
    }
    valueRange = maxValue - minValue;
    if (valueRange < 0.001) valueRange = 1.0;
    
    // Отрисовка
    drawGrid(painter, chartRect);
    drawAxes(painter, chartRect);
    drawData(painter, chartRect);
    drawLabels(painter, chartRect);
    
    if (m_showTooltip) {
        drawTooltip(painter, chartRect);
    }
    
    // Заголовок
    painter.setPen(QPen(m_textColor, 1));
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    QString title = "График показаний";
    if (!m_unit.isEmpty()) {
        title += " (" + m_unit + ")";
    }
    painter.drawText(rect().adjusted(0, 5, 0, 0), Qt::AlignHCenter | Qt::AlignTop, title);
}

void SensorChartWidget::drawGrid(QPainter &painter, const QRect &chartRect) {
    painter.setPen(QPen(m_gridColor, 1, Qt::DashLine));
    
    // Горизонтальные линии
    for (int i = 0; i <= 4; ++i) {
        int y = chartRect.top() + (chartRect.height() * i / 4);
        painter.drawLine(chartRect.left(), y, chartRect.right(), y);
    }
    
    // Вертикальные линии
    for (int i = 0; i <= 4; ++i) {
        int x = chartRect.left() + (chartRect.width() * i / 4);
        painter.drawLine(x, chartRect.top(), x, chartRect.bottom());
    }
}

void SensorChartWidget::drawAxes(QPainter &painter, const QRect &chartRect) {
    painter.setPen(QPen(m_textColor, 1));
    
    // Оси X и Y
    painter.drawLine(chartRect.left(), chartRect.bottom(), chartRect.right(), chartRect.bottom());
    painter.drawLine(chartRect.left(), chartRect.top(), chartRect.left(), chartRect.bottom());
    
    // Стрелки на осях
    painter.drawLine(chartRect.right() - 5, chartRect.bottom() - 5, chartRect.right(), chartRect.bottom());
    painter.drawLine(chartRect.right() - 5, chartRect.bottom() + 5, chartRect.right(), chartRect.bottom());
}

void SensorChartWidget::drawData(QPainter &painter, const QRect &chartRect) {
    if (m_readings.isEmpty()) return;
    
    // Определяем диапазоны
    qint64 minTime = m_readings.first().timestamp().toMSecsSinceEpoch();
    qint64 maxTime = m_readings.last().timestamp().toMSecsSinceEpoch();
    
    // Применяем зум и смещение
    qint64 timeRange = maxTime - minTime;
    qint64 visibleRange = timeRange / m_zoomLevel;
    qint64 centerTime = (minTime + maxTime) / 2 + m_offsetX;
    qint64 visibleMinTime = centerTime - visibleRange / 2;
    qint64 visibleMaxTime = centerTime + visibleRange / 2;
    
    // Фильтруем данные для отображения
    QList<SensorReading> visibleReadings;
    for (const SensorReading &reading : m_readings) {
        qint64 time = reading.timestamp().toMSecsSinceEpoch();
        if (time >= visibleMinTime && time <= visibleMaxTime) {
            visibleReadings.append(reading);
        }
    }
    
    if (visibleReadings.isEmpty()) return;
    
    // Вычисляем min/max значений
    double minValue = visibleReadings.first().value();
    double maxValue = visibleReadings.first().value();
    
    for (const SensorReading &reading : visibleReadings) {
        double val = reading.value();
        if (val < minValue) minValue = val;
        if (val > maxValue) maxValue = val;
    }
    
    double valueRange = maxValue - minValue;
    if (valueRange < 0.001) {
        minValue -= 1.0;
        maxValue += 1.0;
    } else {
        double padding = valueRange * 0.1;
        minValue -= padding;
        maxValue += padding;
        if (minValue < 0 && padding > 0) minValue = 0;
    }
    valueRange = maxValue - minValue;
    if (valueRange < 0.001) valueRange = 1.0;
    
    // Отрисовка графика
    if (visibleReadings.size() >= 2) {
        QPainterPath path;
        bool first = true;
        
        for (const SensorReading &reading : visibleReadings) {
            QPointF point = valueToPoint(reading, chartRect, minValue, maxValue, 
                                         visibleMinTime, visibleMaxTime);
            
            if (first) {
                path.moveTo(point);
                first = false;
            } else {
                path.lineTo(point);
            }
        }
        
        painter.setPen(QPen(m_lineColor, 2));
        painter.drawPath(path);
        
        // Заливка под графиком
        QPainterPath fillPath = path;
        fillPath.lineTo(chartRect.right(), chartRect.bottom());
        fillPath.lineTo(chartRect.left(), chartRect.bottom());
        fillPath.closeSubpath();
        
        painter.fillPath(fillPath, QColor(m_lineColor.red(), m_lineColor.green(), 
                                          m_lineColor.blue(), 30));
    }
    
    // Точки данных
    painter.setBrush(m_lineColor);
    painter.setPen(QPen(m_lineColor, 1));
    
    for (const SensorReading &reading : visibleReadings) {
        QPointF point = valueToPoint(reading, chartRect, minValue, maxValue,
                                     visibleMinTime, visibleMaxTime);
        painter.drawEllipse(point, 3, 3);
    }
}

void SensorChartWidget::drawLabels(QPainter &painter, const QRect &chartRect) {
    if (m_readings.isEmpty()) return;
    
    // Определяем диапазоны
    qint64 minTime = m_readings.first().timestamp().toMSecsSinceEpoch();
    qint64 maxTime = m_readings.last().timestamp().toMSecsSinceEpoch();
    qint64 timeRange = maxTime - minTime;
    qint64 visibleRange = timeRange / m_zoomLevel;
    qint64 centerTime = (minTime + maxTime) / 2 + m_offsetX;
    qint64 visibleMinTime = centerTime - visibleRange / 2;
    qint64 visibleMaxTime = centerTime + visibleRange / 2;
    
    // Фильтруем данные для отображения
    QList<SensorReading> visibleReadings;
    for (const SensorReading &reading : m_readings) {
        qint64 time = reading.timestamp().toMSecsSinceEpoch();
        if (time >= visibleMinTime && time <= visibleMaxTime) {
            visibleReadings.append(reading);
        }
    }
    
    if (visibleReadings.isEmpty()) return;
    
    // Вычисляем min/max значений
    double minValue = visibleReadings.first().value();
    double maxValue = visibleReadings.first().value();
    
    for (const SensorReading &reading : visibleReadings) {
        double val = reading.value();
        if (val < minValue) minValue = val;
        if (val > maxValue) maxValue = val;
    }
    
    double valueRange = maxValue - minValue;
    if (valueRange < 0.001) {
        minValue -= 1.0;
        maxValue += 1.0;
    } else {
        double padding = valueRange * 0.1;
        minValue -= padding;
        maxValue += padding;
        if (minValue < 0 && padding > 0) minValue = 0;
    }
    valueRange = maxValue - minValue;
    if (valueRange < 0.001) valueRange = 1.0;
    
    painter.setPen(QPen(m_textColor, 1));
    painter.setFont(QFont("Arial", 8));
    
    // Подписи значений по Y
    for (int i = 0; i <= 4; ++i) {
        double value = maxValue - (maxValue - minValue) * i / 4.0;
        int y = chartRect.top() + (chartRect.height() * i / 4);
        QString label = QString::number(value, 'f', 1);
        painter.drawText(QRect(0, y - 8, chartRect.left() - 5, 16), 
                         Qt::AlignRight | Qt::AlignVCenter, label);
    }
    
    // Подписи времени по X (выбираем несколько точек)
    painter.setFont(QFont("Arial", 7));
    int numLabels = qMin(5, visibleReadings.size());
    int step = visibleReadings.size() / numLabels;
    if (step < 1) step = 1;
    
    for (int i = 0; i < visibleReadings.size(); i += step) {
        const SensorReading &reading = visibleReadings[i];
        QPointF point = valueToPoint(reading, chartRect, minValue, maxValue,
                                     visibleMinTime, visibleMaxTime);
        QString timeStr = reading.timestamp().toString("HH:mm");
        painter.drawText(QRect(point.x() - 20, chartRect.bottom() + 3, 40, 15), 
                         Qt::AlignHCenter | Qt::AlignTop, timeStr);
    }
}

void SensorChartWidget::drawTooltip(QPainter &painter, const QRect &chartRect) {
    if (!m_showTooltip || !m_tooltipReading.isValid()) return;
    
    // Определяем диапазоны
    qint64 minTime = m_readings.first().timestamp().toMSecsSinceEpoch();
    qint64 maxTime = m_readings.last().timestamp().toMSecsSinceEpoch();
    qint64 timeRange = maxTime - minTime;
    qint64 visibleRange = timeRange / m_zoomLevel;
    qint64 centerTime = (minTime + maxTime) / 2 + m_offsetX;
    qint64 visibleMinTime = centerTime - visibleRange / 2;
    qint64 visibleMaxTime = centerTime + visibleRange / 2;
    
    // Вычисляем min/max значений
    double minValue = m_readings.first().value();
    double maxValue = m_readings.first().value();
    for (const SensorReading &reading : m_readings) {
        double val = reading.value();
        if (val < minValue) minValue = val;
        if (val > maxValue) maxValue = val;
    }
    double valueRange = maxValue - minValue;
    if (valueRange < 0.001) {
        minValue -= 1.0;
        maxValue += 1.0;
    } else {
        double padding = valueRange * 0.1;
        minValue -= padding;
        maxValue += padding;
        if (minValue < 0 && padding > 0) minValue = 0;
    }
    valueRange = maxValue - minValue;
    if (valueRange < 0.001) valueRange = 1.0;
    
    QPointF point = valueToPoint(m_tooltipReading, chartRect, minValue, maxValue,
                                 visibleMinTime, visibleMaxTime);
    
    // Рисуем вертикальную линию
    painter.setPen(QPen(Qt::red, 1, Qt::DashLine));
    painter.drawLine(point.x(), chartRect.top(), point.x(), chartRect.bottom());
    
    // Рисуем точку
    painter.setBrush(Qt::red);
    painter.setPen(QPen(Qt::red, 1));
    painter.drawEllipse(point, 5, 5);
    
    // Подсказка
    QString tooltipText = QString("%1\n%2 %3")
        .arg(m_tooltipReading.timestamp().toString("dd.MM HH:mm:ss"))
        .arg(m_tooltipReading.value(), 0, 'f', 2)
        .arg(m_unit);
    
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(QColor(255, 255, 200));
    painter.setFont(QFont("Arial", 8));
    
    QRect textRect = painter.boundingRect(QRect(), Qt::AlignLeft | Qt::AlignTop, tooltipText);
    textRect.adjust(-5, -3, 5, 3);
    textRect.moveTopLeft(QPoint(point.x() - textRect.width() / 2, chartRect.top() + 5));
    
    // Корректируем, чтобы не выходил за границы
    if (textRect.left() < chartRect.left()) {
        textRect.moveLeft(chartRect.left());
    }
    if (textRect.right() > chartRect.right()) {
        textRect.moveRight(chartRect.right());
    }
    
    painter.drawRect(textRect);
    painter.drawText(textRect, Qt::AlignCenter, tooltipText);
}

QPointF SensorChartWidget::valueToPoint(const SensorReading &reading, const QRect &chartRect,
                                        double minValue, double maxValue,
                                        qint64 minTime, qint64 maxTime) const {
    qint64 time = reading.timestamp().toMSecsSinceEpoch();
    double value = reading.value();
    
    double x = chartRect.left() + (time - minTime) * chartRect.width() / (double)(maxTime - minTime);
    double y = chartRect.bottom() - (value - minValue) * chartRect.height() / (maxValue - minValue);
    
    return QPointF(x, y);
}

SensorReading SensorChartWidget::pointToValue(const QPointF &point, const QRect &chartRect,
                                              double minValue, double maxValue,
                                              qint64 minTime, qint64 maxTime) const {
    qint64 time = minTime + (point.x() - chartRect.left()) * (maxTime - minTime) / chartRect.width();
    double value = minValue + (chartRect.bottom() - point.y()) * (maxValue - minValue) / chartRect.height();
    
    SensorReading reading;
    reading.setTimestamp(QDateTime::fromMSecsSinceEpoch(time));
    reading.setValue(value);
    return reading;
}

void SensorChartWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_isPanning) {
        QPointF delta = event->pos() - m_lastPanPoint;
        qint64 minTime = m_readings.first().timestamp().toMSecsSinceEpoch();
        qint64 maxTime = m_readings.last().timestamp().toMSecsSinceEpoch();
        qint64 timeRange = maxTime - minTime;
        m_offsetX += delta.x() * timeRange / (m_zoomLevel * width());
        m_lastPanPoint = event->pos();
        updateChart();
        return;
    }
    
    // Обновляем подсказку
    const int leftMargin = 50;
    const int rightMargin = 20;
    const int topMargin = 30;
    const int bottomMargin = 35;
    QRect chartRect = rect().adjusted(leftMargin, topMargin, -rightMargin, -bottomMargin);
    
    if (chartRect.contains(event->pos())) {
        // Определяем диапазоны
        qint64 minTime = m_readings.first().timestamp().toMSecsSinceEpoch();
        qint64 maxTime = m_readings.last().timestamp().toMSecsSinceEpoch();
        qint64 timeRange = maxTime - minTime;
        qint64 visibleRange = timeRange / m_zoomLevel;
        qint64 centerTime = (minTime + maxTime) / 2 + m_offsetX;
        qint64 visibleMinTime = centerTime - visibleRange / 2;
        qint64 visibleMaxTime = centerTime + visibleRange / 2;
        
        // Находим ближайшую точку
        QPointF mousePos = event->pos();
        double minDistance = 1e9;
        SensorReading nearestReading;
        
        for (const SensorReading &reading : m_readings) {
            qint64 time = reading.timestamp().toMSecsSinceEpoch();
            if (time < visibleMinTime || time > visibleMaxTime) continue;
            
            double x = chartRect.left() + (time - visibleMinTime) * chartRect.width() / (double)(visibleMaxTime - visibleMinTime);
            double distance = qAbs(x - mousePos.x());
            
            if (distance < minDistance && distance < 20) {
                minDistance = distance;
                nearestReading = reading;
            }
        }
        
        if (nearestReading.isValid()) {
            m_showTooltip = true;
            m_tooltipReading = nearestReading;
            setCursor(Qt::CrossCursor);
        } else {
            m_showTooltip = false;
            setCursor(Qt::ArrowCursor);
        }
    } else {
        m_showTooltip = false;
        setCursor(Qt::ArrowCursor);
    }
    
    updateChart();
    QWidget::mouseMoveEvent(event);
}

void SensorChartWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isPanning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QWidget::mousePressEvent(event);
}

void SensorChartWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
    }
    QWidget::mouseReleaseEvent(event);
}

void SensorChartWidget::wheelEvent(QWheelEvent *event) {
    qreal factor = 1.1;
    if (event->angleDelta().y() < 0) {
        factor = 0.9;
    }
    
    // Зум относительно позиции мыши
    m_zoomLevel *= factor;
    if (m_zoomLevel < 0.1) m_zoomLevel = 0.1;
    if (m_zoomLevel > 10.0) m_zoomLevel = 10.0;
    
    updateChart();
    event->accept();
}

void SensorChartWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateChart();
}