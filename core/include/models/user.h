#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include "../core_global.h"

class CORE_EXPORT User
{
public:
    User();
    explicit User(const QJsonObject &json);

    int userId() const { return m_userId; }
    void setUserId(int id) { m_userId = id; }

    QString username() const { return m_username; }
    void setUsername(const QString &username) { m_username = username; }

    QString passwordHash() const { return m_passwordHash; }
    void setPasswordHash(const QString &hash) { m_passwordHash = hash; }

    QString email() const { return m_email; }
    void setEmail(const QString &email) { m_email = email; }

    QString role() const { return m_role; }
    void setRole(const QString &role) { m_role = role; }

    QDateTime createdAt() const { return m_createdAt; }
    void setCreatedAt(const QDateTime &dt) { m_createdAt = dt; }

    QDateTime lastLogin() const { return m_lastLogin; }
    void setLastLogin(const QDateTime &dt) { m_lastLogin = dt; }

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

private:
    int m_userId = 0;
    QString m_username;
    QString m_passwordHash;
    QString m_email;
    QString m_role = "user";
    QDateTime m_createdAt = QDateTime::currentDateTime();
    QDateTime m_lastLogin;
};

#endif // USER_H