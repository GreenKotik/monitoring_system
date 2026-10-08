#include "models/object.h"
#include <QDebug>

Object::Object()
    : m_positionX(0.0)
    , m_positionY(0.0)
    , m_sizeWidth(100.0)
    , m_sizeHeight(80.0)
    , m_isActive(true)
{
}

Object::~Object() {}

QJsonObject Object::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["object_type_id"] = m_typeId;
    json["object_type_name"] = m_typeName;
    json["parent_object_id"] = m_parentObjectId;
    json["description"] = m_description;
    json["position_x"] = m_positionX;
    json["position_y"] = m_positionY;
    json["size_width"] = m_sizeWidth;
    json["size_height"] = m_sizeHeight;
    json["color_code"] = m_colorCode;
    json["svg_scheme_path"] = m_svgSchemePath;
    json["is_active"] = m_isActive;
    
    if (m_createdAt.isValid()) {
        json["created_at"] = m_createdAt.toString(Qt::ISODate);
    }
    if (m_updatedAt.isValid()) {
        json["updated_at"] = m_updatedAt.toString(Qt::ISODate);
    }
    
    return json;
}

void Object::fromJson(const QJsonObject &json)
{
    m_id = json["id"].toString();
    m_name = json["name"].toString();
    m_typeId = json["object_type_id"].toString();
    m_typeName = json["object_type_name"].toString();
    m_parentObjectId = json["parent_object_id"].toString();
    m_description = json["description"].toString();
    m_positionX = json["position_x"].toDouble(0.0);
    m_positionY = json["position_y"].toDouble(0.0);
    m_sizeWidth = json["size_width"].toDouble(100.0);
    m_sizeHeight = json["size_height"].toDouble(80.0);
    m_colorCode = json["color_code"].toString("#333333");
    m_svgSchemePath = json["svg_scheme_path"].toString();
    m_isActive = json["is_active"].toBool(true);
    
    QString createdAtStr = json["created_at"].toString();
    if (!createdAtStr.isEmpty()) {
        m_createdAt = QDateTime::fromString(createdAtStr, Qt::ISODate);
    }
    
    QString updatedAtStr = json["updated_at"].toString();
    if (!updatedAtStr.isEmpty()) {
        m_updatedAt = QDateTime::fromString(updatedAtStr, Qt::ISODate);
    }
}

bool Object::operator==(const Object &other) const
{
    return m_id == other.m_id;
}

bool Object::operator!=(const Object &other) const
{
    return !(*this == other);
}