#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
//#include <QColor>

class Object
{
public:
    Object();
    ~Object();

    // Геттеры
    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QString typeId() const { return m_typeId; }
    QString typeName() const { return m_typeName; }
    QString parentObjectId() const { return m_parentObjectId; }
    QString description() const { return m_description; }
    double positionX() const { return m_positionX; }
    double positionY() const { return m_positionY; }
    double sizeWidth() const { return m_sizeWidth; }
    double sizeHeight() const { return m_sizeHeight; }
    QString colorCode() const { return m_colorCode; }
    QString svgSchemePath() const { return m_svgSchemePath; }
    QDateTime createdAt() const { return m_createdAt; }
    QDateTime updatedAt() const { return m_updatedAt; }
    bool isActive() const { return m_isActive; }
    bool isValid() const { return !m_id.isEmpty(); }

    // Сеттеры
    void setId(const QString &id) { m_id = id; }
    void setName(const QString &name) { m_name = name; }
    void setTypeId(const QString &typeId) { m_typeId = typeId; }
    void setTypeName(const QString &typeName) { m_typeName = typeName; }
    void setParentObjectId(const QString &parentId) { m_parentObjectId = parentId; }
    void setDescription(const QString &desc) { m_description = desc; }
    void setPositionX(double x) { m_positionX = x; }
    void setPositionY(double y) { m_positionY = y; }
    void setSizeWidth(double w) { m_sizeWidth = w; }
    void setSizeHeight(double h) { m_sizeHeight = h; }
    void setColorCode(const QString &color) { m_colorCode = color; }
    void setSvgSchemePath(const QString &path) { m_svgSchemePath = path; }
    void setCreatedAt(const QDateTime &dt) { m_createdAt = dt; }
    void setUpdatedAt(const QDateTime &dt) { m_updatedAt = dt; }
    void setIsActive(bool active) { m_isActive = active; }

    // JSON сериализация
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

    // Операторы сравнения
    bool operator==(const Object &other) const;
    bool operator!=(const Object &other) const;

private:
    QString m_id;
    QString m_name;
    QString m_typeId;
    QString m_typeName;
    QString m_parentObjectId;
    QString m_description;
    double m_positionX = 0.0;
    double m_positionY = 0.0;
    double m_sizeWidth = 100.0;
    double m_sizeHeight = 80.0;
    QString m_colorCode = "#333333";
    QString m_svgSchemePath;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    bool m_isActive = true;
};
