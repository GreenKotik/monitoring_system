#include "models/user.h"

User::User()
{
}

User::User(const QJsonObject &json)
{
    fromJson(json);
}

QJsonObject User::toJson() const
{
    QJsonObject json;
    json["user_id"] = m_userId;
    json["username"] = m_username;
    json["email"] = m_email;
    json["role"] = m_role;
    json["created_at"] = m_createdAt.toString(Qt::ISODate);
    if (m_lastLogin.isValid()) {
        json["last_login"] = m_lastLogin.toString(Qt::ISODate);
    }
    return json;
}

void User::fromJson(const QJsonObject &json)
{
    m_userId = json["user_id"].toInt();
    m_username = json["username"].toString();
    m_email = json["email"].toString();
    m_role = json["role"].toString();

    QString createdAt = json["created_at"].toString();
    if (!createdAt.isEmpty()) {
        m_createdAt = QDateTime::fromString(createdAt, Qt::ISODate);
    }

    QString lastLogin = json["last_login"].toString();
    if (!lastLogin.isEmpty()) {
        m_lastLogin = QDateTime::fromString(lastLogin, Qt::ISODate);
    }
}