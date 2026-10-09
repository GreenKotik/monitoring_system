#include "apiclient.h"
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
    , m_baseUrl("http://localhost:8080/api")
    , m_authenticated(false)
{
    connect(m_nam, &QNetworkAccessManager::finished, 
            this, &ApiClient::onReplyFinished);
}

ApiClient::~ApiClient() {}

bool ApiClient::login(const QString &username, const QString &password) {
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    sendRequest("POST", "/auth/login", data);
    return true;
}

void ApiClient::logout() {
    m_token.clear();
    m_authenticated = false;
}

bool ApiClient::isAuthenticated() const {
    return m_authenticated && !m_token.isEmpty();
}

void ApiClient::getObjects() {
    sendRequest("GET", "/objects");
}

void ApiClient::getObject(const QString &objectId) {
    sendRequest("GET", "/objects/" + objectId);
}

void ApiClient::getChildObjects(const QString &parentId) {
    sendRequest("GET", "/objects/children/" + parentId);
}

void ApiClient::getSensors(const QString &objectId) {
    QString endpoint = "/sensors";
    if (!objectId.isEmpty()) {
        endpoint += "?object_id=" + objectId;
    }
    sendRequest("GET", endpoint);
}

void ApiClient::getSensor(const QString &sensorId) {
    sendRequest("GET", "/sensors/" + sensorId);
}

void ApiClient::getSensorHistory(const QString &sensorId, const QString &range) {
    sendRequest("GET", "/sensors/" + sensorId + "/history?range=" + range);
}

void ApiClient::sendRequest(const QString &method, const QString &endpoint, 
                            const QJsonObject &data) {
    QUrl url(m_baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (m_authenticated && !m_token.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + m_token).toUtf8());
    }

    QNetworkReply *reply = nullptr;
    if (method == "GET") {
        reply = m_nam->get(request);
    } else if (method == "POST") {
        reply = m_nam->post(request, QJsonDocument(data).toJson());
    } else if (method == "PUT") {
        reply = m_nam->put(request, QJsonDocument(data).toJson());
    } else if (method == "DELETE") {
        reply = m_nam->deleteResource(request);
    }

    if (reply) {
        reply->setProperty("endpoint", endpoint);
        reply->setProperty("method", method);
    }
}

void ApiClient::onReplyFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonObject data = parseReply(reply);
    QString endpoint = reply->property("endpoint").toString();
    
    if (endpoint == "/auth/login") {
        if (data.contains("token")) {
            m_token = data["token"].toString();
            m_authenticated = true;
            emit loginSuccess(m_token);
        } else {
            emit loginFailed(data["error"].toString("Unknown error"));
        }
    } else if (endpoint.startsWith("/objects")) {
        handleObjectsResponse(data);
    } else if (endpoint.startsWith("/sensors")) {
        if (endpoint.contains("/history")) {
            handleSensorHistoryResponse(data);
        } else {
            handleSensorsResponse(data);
        }
    }

    reply->deleteLater();
}

QJsonObject ApiClient::parseReply(QNetworkReply *reply) {
    QByteArray response = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);
    if (doc.isObject()) {
        return doc.object();
    }
    return QJsonObject();
}

void ApiClient::handleObjectsResponse(const QJsonObject &data) {
    QList<Object> objects;
    QJsonArray items = data["objects"].toArray();
    
    for (const QJsonValue &val : items) {
        Object obj;
        obj.fromJson(val.toObject());
        objects.append(obj);
    }
    
    emit objectsLoaded(objects);
}

void ApiClient::handleSensorsResponse(const QJsonObject &data) {
    QList<Sensor> sensors;
    QJsonArray items = data["sensors"].toArray();
    
    for (const QJsonValue &val : items) {
        Sensor sensor;
        sensor.fromJson(val.toObject());
        sensors.append(sensor);
    }
    
    emit sensorsLoaded(sensors);
}

void ApiClient::handleSensorHistoryResponse(const QJsonObject &data) {
    QList<SensorReading> readings;
    QJsonArray items = data["readings"].toArray();
    
    for (const QJsonValue &val : items) {
        SensorReading reading;
        reading.fromJson(val.toObject());
        readings.append(reading);
    }
    
    emit sensorHistoryLoaded(readings);
}