#include "models/objecttype.h"

ObjectType::ObjectType()
{
}

ObjectType::ObjectType(const QJsonObject &json)
{
    fromJson(json);
}

QJsonObject ObjectType::toJson() const
{
    QJsonObject json;
    json["type_id"] = m_typeId;
    json["type_code"] = m_typeCode;
    json["type_name"] = m_typeName;
    json["can_have_children"] = m_canHaveChildren;
    json["icon_name"] = m_iconName;
    json["color_code"] = m_colorCode;
    json["svg_icon_path"] = m_svgIconPath;
    return json;
}

void ObjectType::fromJson(const QJsonObject &json)
{
    m_typeId = json["type_id"].toInt();
    m_typeCode = json["type_code"].toString();
    m_typeName = json["type_name"].toString();
    m_canHaveChildren = json["can_have_children"].toBool();
    m_iconName = json["icon_name"].toString();
    m_colorCode = json["color_code"].toString();
    m_svgIconPath = json["svg_icon_path"].toString();
}