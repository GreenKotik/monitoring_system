#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>

class ApiClient : public QObject
{
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);
    ~ApiClient();

    void setBaseUrl(const QString &url);
    void setToken(const QString &token);
    QString token() const;
    bool isAuthenticated() const;

    // Аутентификация
    void login(const QString &username, const QString &password);
    void logout();

    // Объекты
    void getObjects(bool rootOnly = false);
    void getObject(const QString &objectId);
    void createObject(const QJsonObject &data);
    void updateObject(const QString &objectId, const QJsonObject &data);
    void deleteObject(const QString &objectId);
    void getObjectTree();

    // Датчики
    void getSensors(const QString &objectId = QString());
    void getSensor(const QString &sensorId);
    void getSensorHistory(const QString &sensorId, int count = 100);
    void addReading(const QString &sensorId, qreal value);
    void createSensor(const QJsonObject &data);
    void updateSensor(const QString &sensorId, const QJsonObject &data);
    void deleteSensor(const QString &sensorId);

    // Типы
    void getObjectTypes();
    void getSensorTypes();

    // Карты
    void getMap(const QString &mapId);
    void getScheme(const QString &schemeId);

signals:
    // Общие
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
    void objectTreeReceived(const QJsonArray &tree);

    // Датчики
    void sensorsReceived(const QJsonArray &sensors);
    void sensorReceived(const QJsonObject &sensor);
    void sensorHistoryReceived(const QJsonArray &history);
    void readingAdded(const QJsonObject &reading);
    void sensorCreated(const QJsonObject &sensor);
    void sensorUpdated(const QJsonObject &sensor);
    void sensorDeleted(const QString &sensorId);

    // Типы
    void objectTypesReceived(const QJsonArray &types);
    void sensorTypesReceived(const QJsonArray &types);

    // Карты
    void mapReceived(const QByteArray &data);
    void schemeReceived(const QByteArray &data);

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

#endif // APICLIENT_H