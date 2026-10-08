#ifndef OBJECTTYPE_H
#define OBJECTTYPE_H

#include <QString>
#include <QJsonObject>
#include "../core_global.h"

class CORE_EXPORT ObjectType
{
public:
    ObjectType();
    explicit ObjectType(const QJsonObject &json);

    int typeId() const { return m_typeId; }
    void setTypeId(int id) { m_typeId = id; }

    QString typeCode() const { return m_typeCode; }
    void setTypeCode(const QString &code) { m_typeCode = code; }

    QString typeName() const { return m_typeName; }
    void setTypeName(const QString &name) { m_typeName = name; }

    bool canHaveChildren() const { return m_canHaveChildren; }
    void setCanHaveChildren(bool can) { m_canHaveChildren = can; }

    QString iconName() const { return m_iconName; }
    void setIconName(const QString &name) { m_iconName = name; }

    QString colorCode() const { return m_colorCode; }
    void setColorCode(const QString &code) { m_colorCode = code; }

    QString svgIconPath() const { return m_svgIconPath; }
    void setSvgIconPath(const QString &path) { m_svgIconPath = path; }

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

private:
    int m_typeId = 0;
    QString m_typeCode;
    QString m_typeName;
    bool m_canHaveChildren = false;
    QString m_iconName;
    QString m_colorCode;
    QString m_svgIconPath;
};

#endif // OBJECTTYPE_H