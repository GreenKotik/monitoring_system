#include "controllers/mapcontroller.h"
#include "database/dbmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

void MapController::getMap(const HttpRequest &request, HttpResponse &response) {
    QString mapId = request.getParam("id");
    if (mapId.isEmpty()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\": \"Map ID is required\"}");
        return;
    }

    // Загружаем карту из файла
    QString mapPath = ":/maps/" + mapId + ".svg";
    QFile file(mapPath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        response.setStatus(200, "OK");
        response.setHeader("Content-Type", "image/svg+xml");
        response.setBody(data);
    } else {
        response.setStatus(404, "Not Found");
        response.setBody("{\"error\": \"Map not found\"}");
    }
}

void MapController::getScheme(const HttpRequest &request, HttpResponse &response) {
    QString schemeId = request.getParam("id");
    if (schemeId.isEmpty()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\": \"Scheme ID is required\"}");
        return;
    }

    // Получаем объект из БД
    Object object = DbManager::instance().getObjectById(schemeId);
    if (!object.isValid()) {
        response.setStatus(404, "Not Found");
        response.setBody("{\"error\": \"Scheme not found\"}");
        return;
    }

    // Загружаем схему из файла
    QString schemePath = object.svgSchemePath();
    if (schemePath.isEmpty()) {
        response.setStatus(404, "Not Found");
        response.setBody("{\"error\": \"Scheme path not configured\"}");
        return;
    }

    QFile file(schemePath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        response.setStatus(200, "OK");
        response.setHeader("Content-Type", "image/svg+xml");
        response.setBody(data);
    } else {
        response.setStatus(404, "Not Found");
        response.setBody("{\"error\": \"Scheme file not found\"}");
    }
}
