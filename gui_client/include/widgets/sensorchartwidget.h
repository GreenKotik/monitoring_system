#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QDateTime>
#include <QMap>
#include "models/sensorreading.h"

/**
 * @brief Виджет для отображения графика показаний датчика
 * 
 * Рисует график с помощью QPainter, без использования QtCharts.
 * Поддерживает:
 * - Отрисовку линии графика
 * - Точки данных
 * - Сетку
 * - Подписи осей
 * - Масштабирование
 */
class SensorChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SensorChartWidget(QWidget *parent = nullptr);
    ~SensorChartWidget();

    /**
     * @brief Установить данные для отображения
     * @param readings Список показаний
     * @param unit Единица измерения
     */
    void setData(const QList<SensorReading> &readings, const QString &unit = "");

    /**
     * @brief Очистить график
     */
    void clear();

    /**
     * @brief Установить цвет линии графика
     */
    void setLineColor(const QColor &color);

    /**
     * @brief Установить цвет фона
     */
    void setBackgroundColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void updateChart();
    void drawGrid(QPainter &painter, const QRect &chartRect);
    void drawAxes(QPainter &painter, const QRect &chartRect);
    void drawData(QPainter &painter, const QRect &chartRect);
    void drawLabels(QPainter &painter, const QRect &chartRect);
    void drawTooltip(QPainter &painter, const QRect &chartRect);
    
    QPointF valueToPoint(const SensorReading &reading, const QRect &chartRect,
                         double minValue, double maxValue,
                         qint64 minTime, qint64 maxTime) const;
    
    SensorReading pointToValue(const QPointF &point, const QRect &chartRect,
                               double minValue, double maxValue,
                               qint64 minTime, qint64 maxTime) const;

    // Данные
    QList<SensorReading> m_readings;
    QString m_unit;
    bool m_hasData = false;

    // Настройки отображения
    QColor m_lineColor = QColor(0, 102, 204);
    QColor m_backgroundColor = Qt::white;
    QColor m_gridColor = QColor(200, 200, 200);
    QColor m_textColor = Qt::black;
    
    // Для панорамирования и зума
    bool m_isPanning = false;
    QPointF m_lastPanPoint;
    double m_zoomLevel = 1.0;
    int m_offsetX = 0;
    
    // Для отображения подсказки
    bool m_showTooltip = false;
    QPointF m_tooltipPos;
    SensorReading m_tooltipReading;
};