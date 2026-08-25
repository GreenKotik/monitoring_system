#ifndef ADMIN_APICLIENT_H
#define ADMIN_APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>

class AdminApiClient : public QObject
{
    Q_OBJECT

public:
    explicit AdminApiClient(QObject *parent = nullptr);
    ~AdminApiClient();

    void setBaseUrl(const QString &url);
    void setToken(const QString &token);

    // Аутентификация
    void login(const QString &username, const QString &password);
    void logout();

    // Объекты
    void getObjects();
    void getObject(const QString &objectId);
    void createObject(const QJsonObject &data);
    void updateObject(const QString &objectId, const QJsonObject &data);
    void deleteObject(const QString &objectId);

    // Датчики
    void getSensors(const QString &objectId = QString());
    void getSensor(const QString &sensorId);
    void createSensor(const QJsonObject &data);
    void updateSensor(const QString &sensorId, const QJsonObject &data);
    void deleteSensor(const QString &sensorId);

    // Пользователи
    void getUsers();
    void getUser(const QString &userId);
    void createUser(const QJsonObject &data);
    void updateUser(const QString &userId, const QJsonObject &data);
    void deleteUser(const QString &userId);

signals:
    void errorOccurred(const QString &error);
    void networkError(const QString &error);

    // Аутентификация
    void loginSuccess(const QString &token, const QString &username);
    void loginFailed(const QString &error);
    void logoutSuccess();

    // Объекты
    void objectsReceived(const QJsonArray &objects);
    void objectReceived(const QJsonObject &object);
    void objectCreated(const QJsonObject &object);
    void objectUpdated(const QJsonObject &object);
    void objectDeleted(const QString &objectId);

    // Датчики
    void sensorsReceived(const QJsonArray &sensors);
    void sensorReceived(const QJsonObject &sensor);
    void sensorCreated(const QJsonObject &sensor);
    void sensorUpdated(const QJsonObject &sensor);
    void sensorDeleted(const QString &sensorId);

    // Пользователи
    void usersReceived(const QJsonArray &users);
    void userReceived(const QJsonObject &user);
    void userCreated(const QJsonObject &user);
    void userUpdated(const QJsonObject &user);
    void userDeleted(const QString &userId);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    void sendRequest(const QString &method, const QString &path,
                     const QJsonObject &data = QJsonObject(),
                     bool authenticated = true);
    void handleResponse(QNetworkReply *reply, const QString &requestType);

    QNetworkAccessManager *m_manager;
    QString m_baseUrl;
    QString m_token;
    QMap<QNetworkReply*, QString> m_pendingRequests;
    int m_timeoutMs = 30000;
};

#endif // ADMIN_APICLIENT_H