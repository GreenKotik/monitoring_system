#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "models/object.h"
#include "models/sensor.h"
#include "models/sensorreading.h"

class ApiClient : public QObject
{
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);
    ~ApiClient();

    // Аутентификация
    bool login(const QString &username, const QString &password);
    void logout();
    bool isAuthenticated() const;

    // Объекты
    void getObjects();
    void getObject(const QString &objectId);
    void getChildObjects(const QString &parentId);
    void createObject(const Object &object);
    void updateObject(const Object &object);
    void deleteObject(const QString &objectId);

    // Сенсоры
    void getSensors(const QString &objectId = QString());
    void getSensor(const QString &sensorId);
    void getSensorHistory(const QString &sensorId, const QString &range = "day");
    void createSensor(const Sensor &sensor);
    void updateSensor(const Sensor &sensor);
    void deleteSensor(const QString &sensorId);

signals:
    void loginSuccess(const QString &token);
    void loginFailed(const QString &error);
    void objectsLoaded(const QList<Object> &objects);
    void objectLoaded(const Object &object);
    void childObjectsLoaded(const QList<Object> &children);
    void sensorsLoaded(const QList<Sensor> &sensors);
    void sensorLoaded(const Sensor &sensor);
    void sensorHistoryLoaded(const QList<SensorReading> &readings);
    void errorOccurred(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    void sendRequest(const QString &method, const QString &endpoint, 
                    const QJsonObject &data = QJsonObject());
    QJsonObject parseReply(QNetworkReply *reply);
    void handleObjectsResponse(const QJsonObject &data);
    void handleSensorsResponse(const QJsonObject &data);
    void handleSensorHistoryResponse(const QJsonObject &data);

    QNetworkAccessManager *m_nam;
    QString m_baseUrl;
    QString m_token;
    bool m_authenticated;
};