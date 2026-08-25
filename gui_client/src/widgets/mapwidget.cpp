#include "widgets/mapwidget.h"
#include <QSvgRenderer>
#include <QPainter>
#include <QDebug>
#include <QJsonArray>

MapWidget::MapWidget(QWidget *parent)
    : QSvgWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setRenderHint(QPainter::Antialiasing);
    m_viewBox = QRectF(0, 0, 1920, 1080);
}

MapWidget::~MapWidget()
{
}

void MapWidget::loadMap(const QString &filePath)
{
    load(filePath);
    QSvgRenderer *renderer = this->renderer();
    if (renderer) {
        QRectF viewBox = renderer->viewBox();
        if (!viewBox.isEmpty()) {
            m_viewBox = viewBox;
        }
    }
    update();
}

void MapWidget::loadScheme(const QString &filePath)
{
    load(filePath);
    update();
}

void MapWidget::setObjects(const QJsonArray &objects)
{
    m_objects = objects;
    renderObjects();
}

void MapWidget::setSensors(const QJsonArray &sensors)
{
    m_sensors = sensors;
    renderSensors();
}

void MapWidget::setInteractive(bool enabled)
{
    m_isInteractive = enabled;
}

void MapWidget::renderObjects()
{
    update();
}

void MapWidget::renderSensors()
{
    update();
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    QSvgWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Отрисовка объектов
    for (const QJsonValue &value : m_objects) {
        renderObject(value.toObject());
    }

    // Отрисовка датчиков
    for (const QJsonValue &value : m_sensors) {
        renderSensor(value.toObject());
    }
}

void MapWidget::renderObject(const QJsonObject &obj)
{
    QPainter painter(this);

    QString objectId = obj["object_id"].toString();
    QString name = obj["name"].toString();
    qreal x = obj["position_x"].toDouble();
    qreal y = obj["position_y"].toDouble();
    qreal width = obj["size_width"].toDouble();
    qreal height = obj["size_height"].toDouble();

    // Конвертируем координаты из SVG в экранные
    QPointF topLeft = mapToSvg(QPointF(x, y));
    QPointF bottomRight = mapToSvg(QPointF(x + width, y + height));

    QRectF rect(topLeft, bottomRight);

    // Рисуем прямоугольник объекта
    painter.setBrush(QBrush(QColor("#333333"), Qt::SolidPattern));
    painter.setPen(QPen(QColor("#FFFFFF"), 2));
    painter.drawRoundedRect(rect, 5, 5);

    // Рисуем название
    painter.setPen(QColor("#FFFFFF"));
    painter.setFont(QFont("Arial", 10));
    painter.drawText(rect, Qt::AlignCenter, name);

    // Подсветка при наведении
    if (objectId == m_highlightedObjectId) {
        painter.setPen(QPen(QColor("#89B4FA"), 3));
        painter.drawRoundedRect(rect.adjusted(-2, -2, 2, 2), 5, 5);
    }
}

void MapWidget::renderSensor(const QJsonObject &sensor)
{
    QPainter painter(this);

    QString sensorId = sensor["sensor_id"].toString();
    QString name = sensor["name"].toString();
    qreal x = sensor["position_x"].toDouble();
    qreal y = sensor["position_y"].toDouble();
    qreal value = sensor["last_value"].toDouble();
    QString color = sensor["sensor_type"].toObject()["color_code"].toString("#FF9800");

    QPointF center = mapToSvg(QPointF(x, y));

    // Рисуем круг датчика
    painter.setBrush(QBrush(QColor(color), Qt::SolidPattern));
    painter.setPen(QPen(QColor("#FFFFFF"), 2));
    painter.drawEllipse(center, 12, 12);

    // Рисуем значение
    painter.setPen(QColor("#FFFFFF"));
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    painter.drawText(center + QPointF(-10, -15), QString::number(value));

    // Рисуем название
    painter.setPen(QColor("#333333"));
    painter.setFont(QFont("Arial", 9));
    painter.drawText(center + QPointF(18, 4), name);

    // Подсветка при наведении
    if (sensorId == m_highlightedSensorId) {
        painter.setPen(QPen(QColor("#89B4FA"), 2));
        painter.drawEllipse(center, 16, 16);
    }
}

QPointF MapWidget::mapToSvg(const QPointF &point)
{
    qreal x = point.x() / width() * m_viewBox.width() + m_viewBox.x();
    qreal y = point.y() / height() * m_viewBox.height() + m_viewBox.y();
    return QPointF(x, y);
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_isInteractive) {
        QSvgWidget::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QSvgWidget::mousePressEvent(event);
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_isInteractive) {
        QSvgWidget::mouseMoveEvent(event);
        return;
    }

    if (m_isDragging) {
        QPointF delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();

        qreal dx = delta.x() * (m_viewBox.width() / width());
        qreal dy = delta.y() * (m_viewBox.height() / height());

        m_viewBox.moveLeft(m_viewBox.x() - dx);
        m_viewBox.moveTop(m_viewBox.y() - dy);

        QSvgRenderer *renderer = this->renderer();
        if (renderer) {
            renderer->setViewBox(m_viewBox);
        }
        update();
    }
    QSvgWidget::mouseMoveEvent(event);
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_isInteractive) {
        QSvgWidget::mouseReleaseEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        setCursor(Qt::ArrowCursor);
    }
    QSvgWidget::mouseReleaseEvent(event);
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    if (!m_isInteractive) {
        QSvgWidget::wheelEvent(event);
        return;
    }

    QPointF mousePos = event->position();
    QPointF svgPos = mapToSvg(mousePos);

    qreal factor = event->angleDelta().y() > 0 ? 0.9 : 1.1;
    qreal newScale = m_scale * factor;

    if (newScale < MIN_SCALE) factor = MIN_SCALE / m_scale;
    if (newScale > MAX_SCALE) factor = MAX_SCALE / m_scale;
    if (factor == 1.0) return;

    m_scale *= factor;

    qreal newWidth = m_viewBox.width() * factor;
    qreal newHeight = m_viewBox.height() * factor;
    qreal newX = svgPos.x() - (svgPos.x() - m_viewBox.x()) * factor;
    qreal newY = svgPos.y() - (svgPos.y() - m_viewBox.y()) * factor;

    m_viewBox.setRect(newX, newY, newWidth, newHeight);

    QSvgRenderer *renderer = this->renderer();
    if (renderer) {
        renderer->setViewBox(m_viewBox);
    }
    update();

    event->accept();
}

void MapWidget::resetView()
{
    m_viewBox = QRectF(0, 0, 1920, 1080);
    m_scale = 1.0;
    QSvgRenderer *renderer = this->renderer();
    if (renderer) {
        renderer->setViewBox(m_viewBox);
    }
    update();
}

void MapWidget::zoomIn()
{
    QPointF center = m_viewBox.center();
    qreal factor = 0.8;
    qreal newWidth = m_viewBox.width() * factor;
    qreal newHeight = m_viewBox.height() * factor;
    m_viewBox.setRect(center.x() - newWidth/2, center.y() - newHeight/2, newWidth, newHeight);

    QSvgRenderer *renderer = this->renderer();
    if (renderer) {
        renderer->setViewBox(m_viewBox);
    }
    update();
}

void MapWidget::zoomOut()
{
    QPointF center = m_viewBox.center();
    qreal factor = 1.2;
    qreal newWidth = m_viewBox.width() * factor;
    qreal newHeight = m_viewBox.height() * factor;
    m_viewBox.setRect(center.x() - newWidth/2, center.y() - newHeight/2, newWidth, newHeight);

    QSvgRenderer *renderer = this->renderer();
    if (renderer) {
        renderer->setViewBox(m_viewBox);
    }
    update();
}

void MapWidget::fitToView()
{
    resetView();
}

void MapWidget::highlightObject(const QString &objectId)
{
    m_highlightedObjectId = objectId;
    update();
}

void MapWidget::highlightSensor(const QString &sensorId)
{
    m_highlightedSensorId = sensorId;
    update();
}