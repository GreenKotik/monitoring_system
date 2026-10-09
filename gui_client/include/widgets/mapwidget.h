#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>
#include <QMap>
#include "models/object.h"

class MapWidget : public QGraphicsView
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();

    void setObjects(const QList<Object> &objects);
    void filterObjects(const QString &text);
    void clear();

signals:
    void objectSelected(const QString &objectId);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void loadMap();
    void createFallbackMap();
    void createObjectItems();

    QGraphicsScene *m_scene;
    QGraphicsSvgItem *m_mapItem;
    QMap<QString, QGraphicsItemGroup*> m_objectItems;
    QList<Object> m_objects;
    QString m_filterText;

    bool m_isPanning;
    QPointF m_lastPanPoint;
    qreal m_scale;
};
