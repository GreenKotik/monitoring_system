#include "controllers/objectcontroller.h"
#include <QJsonDocument>
#include <QJsonParseError>

void ObjectController::list(const HttpRequest &req, HttpResponse &res)
{
    bool rootOnly = req.queryParam("root") == "true";
    QList<Object> objects;

    if (rootOnly) {
        objects = DbManager::instance().getRootObjects();
    } else {
        objects = DbManager::instance().getObjects();
    }

    QJsonArray array;
    for (const Object &obj : objects) {
        array.append(obj.toJson());
    }

    QJsonObject response;
    response["status"] = "success";
    response["data"] = array;

    res.setJson(QJsonDocument(response).toJson());
}

void ObjectController::get(const HttpRequest &req, HttpResponse &res)
{
    QString objectId = req.pathParam("id");
    if (objectId.isEmpty()) {
        res.setStatus(HttpResponse::BAD_REQUEST);
        res.setJson(R"({"error": "Object ID is required"})");
        return;
    }

    Object object = DbManager::instance().getObject(objectId);
    if (object.objectId().isEmpty()) {
        res.setStatus(HttpResponse::NOT_FOUND);
        res.setJson(R"({"error": "Object not found"})");
        return;
    }

    QJsonObject response;
    response["status"] = "success";
    response["data"] = object.toJson();

    res.setJson(QJsonDocument(response).toJson());
}

void ObjectController::create(const HttpRequest &req, HttpResponse &res)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(req.body(), &error);

    if (error.error != QJsonParseError::NoError) {
        res.setStatus(HttpResponse::BAD_REQUEST);
        res.setJson(R"({"error": "Invalid JSON"})");
        return;
    }

    Object object(doc.object());
    if (object.objectId().isEmpty()) {
        res.setStatus(HttpResponse::BAD_REQUEST);
        res.setJson(R"({"error": "Object ID is required"})");
        return;
    }

    if (DbManager::instance().createObject(object)) {
        QJsonObject response;
        response["status"] = "success";
        response["data"] = object.toJson();
        res.setStatus(HttpResponse::CREATED);
        res.setJson(QJsonDocument(response).toJson());
    } else {
        res.setStatus(HttpResponse::INTERNAL_SERVER_ERROR);
        res.setJson(R"({"error": "Failed to create object"})");
    }
}

void ObjectController::update(const HttpRequest &req, HttpResponse &res)
{
    QString objectId = req.pathParam("id");
    if (objectId.isEmpty()) {
        res.setStatus(HttpResponse::BAD_REQUEST);
        res.setJson(R"({"error": "Object ID is required"})");
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(req.body(), &error);

    if (error.error != QJsonParseError::NoError) {
        res.setStatus(HttpResponse::BAD_REQUEST);
        res.setJson(R"({"error": "Invalid JSON"})");
        return;
    }

    Object object(doc.object());
    object.setObjectId(objectId);

    if (DbManager::instance().updateObject(object)) {
        QJsonObject response;
        response["status"] = "success";
        response["data"] = object.toJson();
        res.setJson(QJsonDocument(response).toJson());
    } else {
        res.setStatus(HttpResponse::INTERNAL_SERVER_ERROR);
        res.setJson(R"({"error": "Failed to update object"})");
    }
}

void ObjectController::remove(const HttpRequest &req, HttpResponse &res)
{
    QString objectId = req.pathParam("id");
    if (objectId.isEmpty()) {
        res.setStatus(HttpResponse::BAD_REQUEST);
        res.setJson(R"({"error": "Object ID is required"})");
        return;
    }

    if (DbManager::instance().deleteObject(objectId)) {
        QJsonObject response;
        response["status"] = "success";
        response["message"] = "Object deleted";
        res.setJson(QJsonDocument(response).toJson());
    } else {
        res.setStatus(HttpResponse::INTERNAL_SERVER_ERROR);
        res.setJson(R"({"error": "Failed to delete object"})");
    }
}

void ObjectController::getTree(const HttpRequest &req, HttpResponse &res)
{
    QList<Object> objects = DbManager::instance().getObjects();
    QList<Object> tree = Object::buildTree(objects);

    QJsonArray array;
    for (const Object &obj : tree) {
        array.append(obj.toJson());
    }

    QJsonObject response;
    response["status"] = "success";
    response["data"] = array;

    res.setJson(QJsonDocument(response).toJson());
}

void ObjectController::types(const HttpRequest &req, HttpResponse &res)
{
    QList<ObjectType> types = DbManager::instance().getObjectTypes();

    QJsonArray array;
    for (const ObjectType &type : types) {
        array.append(type.toJson());
    }

    QJsonObject response;
    response["status"] = "success";
    response["data"] = array;

    res.setJson(QJsonDocument(response).toJson());
}