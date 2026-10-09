#include "widgets/mapwidget.h"
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QFile>
#include <QSvgRenderer>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QDebug>

MapWidget::MapWidget(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(new QGraphicsScene(this))
    , m_isPanning(false)
    , m_scale(1.0)
{
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    loadMap();
}

MapWidget::~MapWidget() {}

void MapWidget::loadMap() {
    // ИСПРАВЛЕНО: правильная загрузка SVG
    QSvgRenderer *renderer = new QSvgRenderer(this);
    QFile mapFile(":/maps/russia.svg");

    if (mapFile.exists()) {
        // Читаем содержимое файла в QByteArray
        if (mapFile.open(QIODevice::ReadOnly)) {
            QByteArray svgData = mapFile.readAll();
            mapFile.close();

            if (renderer->load(svgData)) {
                m_mapItem = new QGraphicsSvgItem();
                m_mapItem->setSharedRenderer(renderer);
                m_scene->addItem(m_mapItem);
                m_scene->setSceneRect(m_mapItem->boundingRect());
                qDebug() << "Map loaded successfully";
            } else {
                qDebug() << "Failed to load SVG data";
                createFallbackMap();
            }
        } else {
            qDebug() << "Failed to open map file";
            createFallbackMap();
        }
    } else {
        qDebug() << "Map file not found, using fallback";
        createFallbackMap();
    }

    fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

void MapWidget::createFallbackMap() {
    QGraphicsRectItem *rect = m_scene->addRect(0, 0, 1920, 1080);
    rect->setBrush(QColor(232, 244, 248));
    QGraphicsTextItem *text = m_scene->addText("Карта не загружена");
    text->setPos(960 - 100, 500);
}

void MapWidget::setObjects(const QList<Object> &objects) {
    m_objects = objects;
    createObjectItems();
}

void MapWidget::createObjectItems() {
    // Очищаем старые объекты
    qDeleteAll(m_objectItems);
    m_objectItems.clear();

    for (const Object &obj : m_objects) {
        if (!obj.parentObjectId().isEmpty()) continue;

        QGraphicsItemGroup *group = new QGraphicsItemGroup();

        QGraphicsRectItem *rect = new QGraphicsRectItem(
            obj.positionX() - obj.sizeWidth()/2,
            obj.positionY() - obj.sizeHeight()/2,
            obj.sizeWidth(),
            obj.sizeHeight()
        );
        rect->setBrush(QColor(obj.colorCode()));
        rect->setPen(QPen(Qt::white, 2));
        rect->setData(0, obj.id());
        group->addToGroup(rect);

        QGraphicsTextItem *text = new QGraphicsTextItem(obj.name());
        text->setDefaultTextColor(Qt::white);
        text->setPos(obj.positionX() - 30, obj.positionY() + obj.sizeHeight()/2 + 5);
        text->setData(0, obj.id());
        group->addToGroup(text);

        m_objectItems[obj.id()] = group;
        m_scene->addItem(group);
    }
}

void MapWidget::filterObjects(const QString &text) {
    m_filterText = text;
    for (auto it = m_objectItems.begin(); it != m_objectItems.end(); ++it) {
        bool visible = text.isEmpty() ||
                       it.key().contains(text, Qt::CaseInsensitive);
        it.value()->setVisible(visible);
    }
}

void MapWidget::clear() {
    m_scene->clear();
    m_objectItems.clear();
}

void MapWidget::wheelEvent(QWheelEvent *event) {
    qreal factor = 1.1;
    if (event->angleDelta().y() < 0) {
        factor = 0.9;
    }
    scale(factor, factor);
    m_scale *= factor;
}

void MapWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPointF scenePos = mapToScene(event->pos());
        QGraphicsItem *item = m_scene->itemAt(scenePos, transform());

        if (item) {
            QVariant data = item->data(0);
            if (data.isValid()) {
                QString objectId = data.toString();
                if (!objectId.isEmpty()) {
                    emit objectSelected(objectId);
                    return;
                }
            }
        }

        m_isPanning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsView::mousePressEvent(event);
}

// ИСПРАВЛЕНО: используем translate вместо scrollBar
void MapWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_isPanning) {
        QPointF delta = event->pos() - m_lastPanPoint;
        // Используем translate для панорамирования
        QTransform transform = this->transform();
        transform.translate(delta.x() / transform.m11(), delta.y() / transform.m22());
        setTransform(transform);
        m_lastPanPoint = event->pos();
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MapWidget::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
}

