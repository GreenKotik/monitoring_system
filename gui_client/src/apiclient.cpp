#include "apiclient.h"
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>
#include "core/utils/logger.h"

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &ApiClient::onReplyFinished);
}

ApiClient::~ApiClient()
{
}

void ApiClient::setBaseUrl(const QString &url)
{
    m_baseUrl = url;
}

void ApiClient::setToken(const QString &token)
{
    m_token = token;
}

QString ApiClient::token() const
{
    return m_token;
}

bool ApiClient::isAuthenticated() const
{
    return !m_token.isEmpty();
}

void ApiClient::login(const QString &username, const QString &password)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;

    sendRequest("POST", "/auth/login", data, false);
}

void ApiClient::logout()
{
    sendRequest("POST", "/auth/logout", QJsonObject(), true);
}

void ApiClient::getObjects(bool rootOnly)
{
    QString path = "/objects";
    if (rootOnly) {
        path += "?root=true";
    }
    sendRequest("GET", path);
}

void ApiClient::getObject(const QString &objectId)
{
    sendRequest("GET", "/objects/" + objectId);
}

void ApiClient::createObject(const QJsonObject &data)
{
    sendRequest("POST", "/objects", data);
}

void ApiClient::updateObject(const QString &objectId, const QJsonObject &data)
{
    sendRequest("PUT", "/objects/" + objectId, data);
}

void ApiClient::deleteObject(const QString &objectId)
{
    sendRequest("DELETE", "/objects/" + objectId);
}

void ApiClient::getObjectTree()
{
    sendRequest("GET", "/objects/tree");
}

void ApiClient::getSensors(const QString &objectId)
{
    QString path = "/sensors";
    if (!objectId.isEmpty()) {
        path += "?object_id=" + objectId;
    }
    sendRequest("GET", path);
}

void ApiClient::getSensor(const QString &sensorId)
{
    sendRequest("GET", "/sensors/" + sensorId);
}

void ApiClient::getSensorHistory(const QString &sensorId, int count)
{
    sendRequest("GET", "/sensors/" + sensorId + "/history?count=" + QString::number(count));
}

void ApiClient::addReading(const QString &sensorId, qreal value)
{
    QJsonObject data;
    data["value"] = value;
    sendRequest("POST", "/sensors/" + sensorId + "/reading", data);
}

void ApiClient::createSensor(const QJsonObject &data)
{
    sendRequest("POST", "/sensors", data);
}

void ApiClient::updateSensor(const QString &sensorId, const QJsonObject &data)
{
    sendRequest("PUT", "/sensors/" + sensorId, data);
}

void ApiClient::deleteSensor(const QString &sensorId)
{
    sendRequest("DELETE", "/sensors/" + sensorId);
}

void ApiClient::getObjectTypes()
{
    sendRequest("GET", "/object-types");
}

void ApiClient::getSensorTypes()
{
    sendRequest("GET", "/sensor-types");
}

void ApiClient::getMap(const QString &mapId)
{
    sendRequest("GET", "/maps/" + mapId);
}

void ApiClient::getScheme(const QString &schemeId)
{
    sendRequest("GET", "/schemes/" + schemeId);
}

void ApiClient::sendRequest(const QString &method, const QString &path,
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

void ApiClient::onReplyFinished(QNetworkReply *reply)
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

void ApiClient::handleResponse(QNetworkReply *reply, const QString &requestType)
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

    // Определяем тип ответа по запросу
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
    } else if (requestType.contains("GET /objects/tree")) {
        emit objectTreeReceived(response["data"].toArray());
    } else if (requestType.contains("GET /sensors")) {
        emit sensorsReceived(response["data"].toArray());
    } else if (requestType.contains("GET /sensors/")) {
        emit sensorReceived(response["data"].toObject());
    } else if (requestType.contains("GET /sensors/") && requestType.contains("/history")) {
        emit sensorHistoryReceived(response["data"].toArray());
    } else if (requestType.contains("POST /sensors/") && requestType.contains("/reading")) {
        emit readingAdded(response["data"].toObject());
    } else if (requestType.contains("POST /sensors")) {
        emit sensorCreated(response["data"].toObject());
    } else if (requestType.contains("PUT /sensors/")) {
        emit sensorUpdated(response["data"].toObject());
    } else if (requestType.contains("DELETE /sensors/")) {
        QString id = response["data"].toObject()["sensor_id"].toString();
        emit sensorDeleted(id);
    } else if (requestType.contains("GET /object-types")) {
        emit objectTypesReceived(response["data"].toArray());
    } else if (requestType.contains("GET /sensor-types")) {
        emit sensorTypesReceived(response["data"].toArray());
    } else if (requestType.contains("GET /maps/")) {
        emit mapReceived(data);
    } else if (requestType.contains("GET /schemes/")) {
        emit schemeReceived(data);
    } else {
        qDebug() << "Unknown response type:" << requestType;
    }
}