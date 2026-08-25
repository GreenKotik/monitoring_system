#ifndef OBJECT_H
#define OBJECT_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include "../core_global.h"
#include "objecttype.h"
#include "sensor.h"

class CORE_EXPORT Object
{
public:
    Object();
    explicit Object(const QJsonObject &json);

    // Геттеры и сеттеры
    QString objectId() const { return m_objectId; }
    void setObjectId(const QString &id) { m_objectId = id; }

    int objectTypeId() const { return m_objectTypeId; }
    void setObjectTypeId(int id) { m_objectTypeId = id; }

    QString parentObjectId() const { return m_parentObjectId; }
    void setParentObjectId(const QString &id) { m_parentObjectId = id; }

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString description() const { return m_description; }
    void setDescription(const QString &desc) { m_description = desc; }

    qreal positionX() const { return m_positionX; }
    void setPositionX(qreal x) { m_positionX = x; }

    qreal positionY() const { return m_positionY; }
    void setPositionY(qreal y) { m_positionY = y; }

    qreal sizeWidth() const { return m_sizeWidth; }
    void setSizeWidth(qreal w) { m_sizeWidth = w; }

    qreal sizeHeight() const { return m_sizeHeight; }
    void setSizeHeight(qreal h) { m_sizeHeight = h; }

    QString svgSchemePath() const { return m_svgSchemePath; }
    void setSvgSchemePath(const QString &path) { m_svgSchemePath = path; }

    QString status() const { return m_status; }
    void setStatus(const QString &status) { m_status = status; }

    QDateTime createdAt() const { return m_createdAt; }
    void setCreatedAt(const QDateTime &dt) { m_createdAt = dt; }

    ObjectType objectType() const { return m_objectType; }
    void setObjectType(const ObjectType &type) { m_objectType = type; }

    QList<Object> children() const { return m_children; }
    void setChildren(const QList<Object> &children) { m_children = children; }
    void addChild(const Object &child) { m_children.append(child); }

    QList<Sensor> sensors() const { return m_sensors; }
    void setSensors(const QList<Sensor> &sensors) { m_sensors = sensors; }
    void addSensor(const Sensor &sensor) { m_sensors.append(sensor); }

    bool isRoot() const { return m_parentObjectId.isEmpty(); }
    bool hasChildren() const { return !m_children.isEmpty(); }
    bool hasSensors() const { return !m_sensors.isEmpty(); }

    // Сериализация
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

    // Статические методы для работы с деревом
    static QList<Object> buildTree(const QList<Object> &objects);
    static QList<Object> findRoots(const QList<Object> &objects);
    static QList<Object> findChildren(const QString &parentId, const QList<Object> &objects);
    static Object* findObject(const QString &id, QList<Object> &objects);
    static QList<Object> getPath(const QString &id, const QList<Object> &objects);

private:
    QString m_objectId;
    int m_objectTypeId = 0;
    QString m_parentObjectId;
    QString m_name;
    QString m_description;
    qreal m_positionX = 0.0;
    qreal m_positionY = 0.0;
    qreal m_sizeWidth = 100.0;
    qreal m_sizeHeight = 80.0;
    QString m_svgSchemePath;
    QString m_status = "active";
    QDateTime m_createdAt = QDateTime::currentDateTime();

    ObjectType m_objectType;
    QList<Object> m_children;
    QList<Sensor> m_sensors;
};

#endif // OBJECT_H