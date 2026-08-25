#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QSvgWidget>
#include <QPointF>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>

class MapWidget : public QSvgWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();

    void loadMap(const QString &filePath);
    void loadScheme(const QString &filePath);
    void setObjects(const QJsonArray &objects);
    void setSensors(const QJsonArray &sensors);
    void setInteractive(bool enabled);

    void resetView();
    void zoomIn();
    void zoomOut();
    void fitToView();

    void highlightObject(const QString &objectId);
    void highlightSensor(const QString &sensorId);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void objectClicked(const QString &objectId);
    void sensorClicked(const QString &sensorId);
    void viewChanged(const QRectF &viewBox);

private:
    void renderObjects();
    void renderSensors();
    void renderObject(const QJsonObject &obj);
    void renderSensor(const QJsonObject &sensor);
    QPointF mapToSvg(const QPointF &point);
    QRectF getObjectBounds(const QJsonObject &obj);

    QRectF m_viewBox;
    QPointF m_lastMousePos;
    bool m_isDragging = false;
    bool m_isInteractive = true;

    QJsonArray m_objects;
    QJsonArray m_sensors;
    QString m_highlightedObjectId;
    QString m_highlightedSensorId;

    qreal m_scale = 1.0;
    const qreal MIN_SCALE = 0.5;
    const qreal MAX_SCALE = 2.0;

    QMap<QString, QPixmap> m_iconCache;
};

#endif // MAPWIDGET_H