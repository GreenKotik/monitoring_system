#include "models/sensortype.h"

SensorType::SensorType()
{
}

SensorType::SensorType(const QJsonObject &json)
{
    fromJson(json);
}

QJsonObject SensorType::toJson() const
{
    QJsonObject json;
    json["type_id"] = m_typeId;
    json["type_code"] = m_typeCode;
    json["type_name"] = m_typeName;
    json["unit"] = m_unit;
    json["color_code"] = m_colorCode;
    json["icon_name"] = m_iconName;
    json["svg_image_path"] = m_svgImagePath;
    return json;
}

void SensorType::fromJson(const QJsonObject &json)
{
    m_typeId = json["type_id"].toInt();
    m_typeCode = json["type_code"].toString();
    m_typeName = json["type_name"].toString();
    m_unit = json["unit"].toString();
    m_colorCode = json["color_code"].toString();
    m_iconName = json["icon_name"].toString();
    m_svgImagePath = json["svg_image_path"].toString();
}