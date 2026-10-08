#ifndef SENSORTYPE_H
#define SENSORTYPE_H

#include <QString>
#include <QJsonObject>
#include "../core_global.h"

class CORE_EXPORT SensorType
{
public:
    SensorType();
    explicit SensorType(const QJsonObject &json);

    int typeId() const { return m_typeId; }
    void setTypeId(int id) { m_typeId = id; }

    QString typeCode() const { return m_typeCode; }
    void setTypeCode(const QString &code) { m_typeCode = code; }

    QString typeName() const { return m_typeName; }
    void setTypeName(const QString &name) { m_typeName = name; }

    QString unit() const { return m_unit; }
    void setUnit(const QString &unit) { m_unit = unit; }

    QString colorCode() const { return m_colorCode; }
    void setColorCode(const QString &code) { m_colorCode = code; }

    QString iconName() const { return m_iconName; }
    void setIconName(const QString &name) { m_iconName = name; }

    QString svgImagePath() const { return m_svgImagePath; }
    void setSvgImagePath(const QString &path) { m_svgImagePath = path; }

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

private:
    int m_typeId = 0;
    QString m_typeCode;
    QString m_typeName;
    QString m_unit;
    QString m_colorCode;
    QString m_iconName;
    QString m_svgImagePath;
};

#endif // SENSORTYPE_H