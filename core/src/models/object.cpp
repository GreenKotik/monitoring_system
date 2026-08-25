#include "models/object.h"

Object::Object()
{
}

Object::Object(const QJsonObject &json)
{
    fromJson(json);
}

QJsonObject Object::toJson() const
{
    QJsonObject json;
    json["object_id"] = m_objectId;
    json["object_type_id"] = m_objectTypeId;
    json["parent_object_id"] = m_parentObjectId;
    json["name"] = m_name;
    json["description"] = m_description;
    json["position_x"] = m_positionX;
    json["position_y"] = m_positionY;
    json["size_width"] = m_sizeWidth;
    json["size_height"] = m_sizeHeight;
    json["svg_scheme_path"] = m_svgSchemePath;
    json["status"] = m_status;
    json["created_at"] = m_createdAt.toString(Qt::ISODate);

    json["object_type"] = m_objectType.toJson();

    QJsonArray childrenArray;
    for (const Object &child : m_children) {
        childrenArray.append(child.toJson());
    }
    json["children"] = childrenArray;

    QJsonArray sensorsArray;
    for (const Sensor &sensor : m_sensors) {
        sensorsArray.append(sensor.toJson());
    }
    json["sensors"] = sensorsArray;

    return json;
}

void Object::fromJson(const QJsonObject &json)
{
    m_objectId = json["object_id"].toString();
    m_objectTypeId = json["object_type_id"].toInt();
    m_parentObjectId = json["parent_object_id"].toString();
    m_name = json["name"].toString();
    m_description = json["description"].toString();
    m_positionX = json["position_x"].toDouble();
    m_positionY = json["position_y"].toDouble();
    m_sizeWidth = json["size_width"].toDouble();
    m_sizeHeight = json["size_height"].toDouble();
    m_svgSchemePath = json["svg_scheme_path"].toString();
    m_status = json["status"].toString();

    QString createdAt = json["created_at"].toString();
    if (!createdAt.isEmpty()) {
        m_createdAt = QDateTime::fromString(createdAt, Qt::ISODate);
    }

    if (json.contains("object_type") && json["object_type"].isObject()) {
        m_objectType.fromJson(json["object_type"].toObject());
    }

    if (json.contains("children") && json["children"].isArray()) {
        m_children.clear();
        QJsonArray childrenArray = json["children"].toArray();
        for (const QJsonValue &value : childrenArray) {
            Object child;
            child.fromJson(value.toObject());
            m_children.append(child);
        }
    }

    if (json.contains("sensors") && json["sensors"].isArray()) {
        m_sensors.clear();
        QJsonArray sensorsArray = json["sensors"].toArray();
        for (const QJsonValue &value : sensorsArray) {
            Sensor sensor;
            sensor.fromJson(value.toObject());
            m_sensors.append(sensor);
        }
    }
}

QList<Object> Object::buildTree(const QList<Object> &objects)
{
    QList<Object> result = objects;
    QList<Object> roots = findRoots(result);

    for (Object &root : roots) {
        QList<Object> children = findChildren(root.objectId(), result);
        root.setChildren(children);
    }

    return roots;
}

QList<Object> Object::findRoots(const QList<Object> &objects)
{
    QList<Object> roots;
    for (const Object &obj : objects) {
        if (obj.isRoot()) {
            roots.append(obj);
        }
    }
    return roots;
}

QList<Object> Object::findChildren(const QString &parentId, const QList<Object> &objects)
{
    QList<Object> children;
    for (const Object &obj : objects) {
        if (obj.parentObjectId() == parentId) {
            children.append(obj);
        }
    }
    return children;
}

Object* Object::findObject(const QString &id, QList<Object> &objects)
{
    for (Object &obj : objects) {
        if (obj.objectId() == id) {
            return &obj;
        }
        if (obj.hasChildren()) {
            Object* found = findObject(id, obj.m_children);
            if (found) {
                return found;
            }
        }
    }
    return nullptr;
}

QList<Object> Object::getPath(const QString &id, const QList<Object> &objects)
{
    QList<Object> path;
    Object* obj = const_cast<Object*>(&Object::findObject(id, const_cast<QList<Object>&>(objects)));
    if (!obj) return path;

    while (obj) {
        path.prepend(*obj);
        obj = const_cast<Object*>(&Object::findObject(obj->parentObjectId(), const_cast<QList<Object>&>(objects)));
    }

    return path;
}