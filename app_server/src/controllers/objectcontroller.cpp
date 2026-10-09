#include "controllers/objectcontroller.h"
#include "database/dbmanager.h"
#include "models/object.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>

void ObjectController::list(const HttpRequest &request, HttpResponse &response) {
    QList<Object> objects;

    QString parentId = request.getParam("parentId");
    if (!parentId.isEmpty()) {
        objects = DbManager::instance().getObjects(parentId);
    } else {
        objects = DbManager::instance().getObjects();
    }

    QJsonArray jsonArray;
    for (const Object &obj : objects) {
        jsonArray.append(obj.toJson());
    }

    QJsonObject json;
    json["status"] = "success";
    json["objects"] = jsonArray;

    response.setStatus(200, "OK");
    response.setBody(QJsonDocument(json).toJson());
}

void ObjectController::get(const HttpRequest &request, HttpResponse &response) {
    QString objectId = request.getParam("id");
    if (objectId.isEmpty()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Object ID required\"}");
        return;
    }

    Object object = DbManager::instance().getObject(objectId);
    if (!object.isValid()) {
        response.setStatus(404, "Not Found");
        response.setBody("{\"error\":\"Object not found\"}");
        return;
    }

    QJsonObject json;
    json["status"] = "success";
    json["object"] = object.toJson();

    response.setStatus(200, "OK");
    response.setBody(QJsonDocument(json).toJson());
}

void ObjectController::create(const HttpRequest &request, HttpResponse &response) {
    QJsonDocument doc = QJsonDocument::fromJson(request.body());
    if (!doc.isObject()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Invalid JSON\"}");
        return;
    }

    Object object;
    object.fromJson(doc.object());

    if (object.id().isEmpty()) {
        object.setId(QUuid::createUuid().toString(QUuid::WithoutBraces));
    }

    if (DbManager::instance().createObject(object)) {
        QJsonObject json;
        json["status"] = "success";
        json["message"] = "Object created";
        json["object"] = object.toJson();

        response.setStatus(201, "Created");
        response.setBody(QJsonDocument(json).toJson());
    } else {
        response.setStatus(500, "Internal Server Error");
        response.setBody("{\"error\":\"Failed to create object\"}");
    }
}

void ObjectController::update(const HttpRequest &request, HttpResponse &response) {
    QString objectId = request.getParam("id");
    if (objectId.isEmpty()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Object ID required\"}");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(request.body());
    if (!doc.isObject()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Invalid JSON\"}");
        return;
    }

    Object object;
    object.fromJson(doc.object());
    object.setId(objectId);

    if (DbManager::instance().updateObject(object)) {
        QJsonObject json;
        json["status"] = "success";
        json["message"] = "Object updated";
        json["object"] = object.toJson();

        response.setStatus(200, "OK");
        response.setBody(QJsonDocument(json).toJson());
    } else {
        response.setStatus(500, "Internal Server Error");
        response.setBody("{\"error\":\"Failed to update object\"}");
    }
}

void ObjectController::remove(const HttpRequest &request, HttpResponse &response) {
    QString objectId = request.getParam("id");
    if (objectId.isEmpty()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Object ID required\"}");
        return;
    }

    if (DbManager::instance().deleteObject(objectId)) {
        QJsonObject json;
        json["status"] = "success";
        json["message"] = "Object deleted";

        response.setStatus(200, "OK");
        response.setBody(QJsonDocument(json).toJson());
    } else {
        response.setStatus(500, "Internal Server Error");
        response.setBody("{\"error\":\"Failed to delete object\"}");
    }
}

void ObjectController::getTree(const HttpRequest &request, HttpResponse &response) {
    QList<Object> objects = DbManager::instance().getObjects();

    QJsonArray jsonArray;
    for (const Object &obj : objects) {
        jsonArray.append(obj.toJson());
    }

    QJsonObject json;
    json["status"] = "success";
    json["objects"] = jsonArray;

    response.setStatus(200, "OK");
    response.setBody(QJsonDocument(json).toJson());
}
