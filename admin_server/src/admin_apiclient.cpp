#include "admin_apiclient.h"
#include <QUrl>
#include <QDebug>

AdminApiClient::AdminApiClient(QObject *parent)
    : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &AdminApiClient::onReplyFinished);
}

AdminApiClient::~AdminApiClient()
{
}

void AdminApiClient::setBaseUrl(const QString &url)
{
    m_baseUrl = url;
}

void AdminApiClient::setToken(const QString &token)
{
    m_token = token;
}

void AdminApiClient::login(const QString &username, const QString &password)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    sendRequest("POST", "/auth/login", data, false);
}

void AdminApiClient::logout()
{
    sendRequest("POST", "/auth/logout", QJsonObject(), true);
}

void AdminApiClient::getObjects()
{
    sendRequest("GET", "/objects");
}

void AdminApiClient::getObject(const QString &objectId)
{
    sendRequest("GET", "/objects/" + objectId);
}

void AdminApiClient::createObject(const QJsonObject &data)
{
    sendRequest("POST", "/objects", data);
}

void AdminApiClient::updateObject(const QString &objectId, const QJsonObject &data)
{
    sendRequest("PUT", "/objects/" + objectId, data);
}

void AdminApiClient::deleteObject(const QString &objectId)
{
    sendRequest("DELETE", "/objects/" + objectId);
}

void AdminApiClient::getSensors(const QString &objectId)
{
    QString path = "/sensors";
    if (!objectId.isEmpty()) {
        path += "?object_id=" + objectId;
    }
    sendRequest("GET", path);
}

void AdminApiClient::getSensor(const QString &sensorId)
{
    sendRequest("GET", "/sensors/" + sensorId);
}

void AdminApiClient::createSensor(const QJsonObject &data)
{
    sendRequest("POST", "/sensors", data);
}

void AdminApiClient::updateSensor(const QString &sensorId, const QJsonObject &data)
{
    sendRequest("PUT", "/sensors/" + sensorId, data);
}

void AdminApiClient::deleteSensor(const QString &sensorId)
{
    sendRequest("DELETE", "/sensors/" + sensorId);
}

void AdminApiClient::getUsers()
{
    sendRequest("GET", "/users");
}

void AdminApiClient::getUser(const QString &userId)
{
    sendRequest("GET", "/users/" + userId);
}

void AdminApiClient::createUser(const QJsonObject &data)
{
    sendRequest("POST", "/users", data);
}

void AdminApiClient::updateUser(const QString &userId, const QJsonObject &data)
{
    sendRequest("PUT", "/users/" + userId, data);
}

void AdminApiClient::deleteUser(const QString &userId)
{
    sendRequest("DELETE", "/users/" + userId);
}

void AdminApiClient::sendRequest(const QString &method, const QString &path,
                                const QJsonObject &data, bool authenticated)
{
    if (authenticated && m_token.isEmpty()) {
        emit errorOccurred("Not authenticated");
        return;
    }

    QUrl url(m_baseUrl + path);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (authenticated && !m_token.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + m_token).toUtf8());
    }

    QNetworkReply *reply = nullptr;

    if (method == "GET") {
        reply = m_manager->get(request);
    } else if (method == "POST") {
        reply = m_manager->post(request, QJsonDocument(data).toJson());
    } else if (method == "PUT") {
        reply = m_manager->put(request, QJsonDocument(data).toJson());
    } else if (method == "DELETE") {
        reply = m_manager->deleteResource(request);
    } else {
        emit errorOccurred("Unknown HTTP method: " + method);
        return;
    }

    m_pendingRequests[reply] = method + " " + path;
}

void AdminApiClient::onReplyFinished(QNetworkReply *reply)
{
    if (!reply) return;

    QString requestType = m_pendingRequests.value(reply, "unknown");
    m_pendingRequests.remove(reply);

    if (reply->error() != QNetworkReply::NoError) {
        emit networkError(reply->errorString());
        reply->deleteLater();
        return;
    }

    handleResponse(reply, requestType);
    reply->deleteLater();
}

void AdminApiClient::handleResponse(QNetworkReply *reply, const QString &requestType)
{
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull()) {
        emit errorOccurred("Invalid JSON response");
        return;
    }

    QJsonObject response = doc.object();
    QString status = response["status"].toString();

    if (status != "success") {
        QString error = response["message"].toString("Unknown error");
        emit errorOccurred(error);
        return;
    }

    if (requestType.contains("login")) {
        QString token = response["token"].toString();
        QString username = response["user"].toString();
        setToken(token);
        emit loginSuccess(token, username);
    } else if (requestType.contains("logout")) {
        setToken("");
        emit logoutSuccess();
    } else if (requestType.contains("GET /objects")) {
        emit objectsReceived(response["data"].toArray());
    } else if (requestType.contains("GET /objects/")) {
        emit objectReceived(response["data"].toObject());
    } else if (requestType.contains("POST /objects")) {
        emit objectCreated(response["data"].toObject());
    } else if (requestType.contains("PUT /objects/")) {
        emit objectUpdated(response["data"].toObject());
    } else if (requestType.contains("DELETE /objects/")) {
        QString id = response["data"].toObject()["object_id"].toString();
        emit objectDeleted(id);
    } else if (requestType.contains("GET /sensors")) {
        emit sensorsReceived(response["data"].toArray());
    } else if (requestType.contains("GET /sensors/")) {
        emit sensorReceived(response["data"].toObject());
    } else if (requestType.contains("POST /sensors")) {
        emit sensorCreated(response["data"].toObject());
    } else if (requestType.contains("PUT /sensors/")) {
        emit sensorUpdated(response["data"].toObject());
    } else if (requestType.contains("DELETE /sensors/")) {
        QString id = response["data"].toObject()["sensor_id"].toString();
        emit sensorDeleted(id);
    } else if (requestType.contains("GET /users")) {
        emit usersReceived(response["data"].toArray());
    } else if (requestType.contains("GET /users/")) {
        emit userReceived(response["data"].toObject());
    } else if (requestType.contains("POST /users")) {
        emit userCreated(response["data"].toObject());
    } else if (requestType.contains("PUT /users/")) {
        emit userUpdated(response["data"].toObject());
    } else if (requestType.contains("DELETE /users/")) {
        QString id = response["data"].toObject()["user_id"].toString();
        emit userDeleted(id);
    } else {
        qDebug() << "Unknown response type:" << requestType;
    }
}