#include "controllers/sensorcontroller.h"
#include "database/dbmanager.h"
#include "models/sensor.h"
#include "models/sensorreading.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>

void SensorController::list(const HttpRequest &request, HttpResponse &response) {
    QString objectId = request.getParam("objectId");
    QList<Sensor> sensors = DbManager::instance().getSensors(objectId);

    QJsonArray jsonArray;
    for (const Sensor &sensor : sensors) {
        jsonArray.append(sensor.toJson());
    }

    QJsonObject json;
    json["status"] = "success";
    json["sensors"] = jsonArray;

    response.setStatus(200, "OK");
    response.setBody(QJsonDocument(json).toJson());
}

void SensorController::get(const HttpRequest &request, HttpResponse &response) {
    QString sensorId = request.getParam("id");
    if (sensorId.isEmpty()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Sensor ID required\"}");
        return;
    }

    Sensor sensor = DbManager::instance().getSensor(sensorId);
    if (!sensor.isValid()) {
        response.setStatus(404, "Not Found");
        response.setBody("{\"error\":\"Sensor not found\"}");
        return;
    }

    QJsonObject json;
    json["status"] = "success";
    json["sensor"] = sensor.toJson();

    response.setStatus(200, "OK");
    response.setBody(QJsonDocument(json).toJson());
}

void SensorController::create(const HttpRequest &request, HttpResponse &response) {
    QJsonDocument doc = QJsonDocument::fromJson(request.body());
    if (!doc.isObject()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Invalid JSON\"}");
        return;
    }

    Sensor sensor;
    sensor.fromJson(doc.object());

    if (sensor.id().isEmpty()) {
        sensor.setId(QUuid::createUuid().toString(QUuid::WithoutBraces));
    }

    if (DbManager::instance().createSensor(sensor)) {
        QJsonObject json;
        json["status"] = "success";
        json["message"] = "Sensor created";
        json["sensor"] = sensor.toJson();

        response.setStatus(201, "Created");
        response.setBody(QJsonDocument(json).toJson());
    } else {
        response.setStatus(500, "Internal Server Error");
        response.setBody("{\"error\":\"Failed to create sensor\"}");
    }
}

void SensorController::update(const HttpRequest &request, HttpResponse &response) {
    QString sensorId = request.getParam("id");
    if (sensorId.isEmpty()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Sensor ID required\"}");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(request.body());
    if (!doc.isObject()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Invalid JSON\"}");
        return;
    }

    Sensor sensor;
    sensor.fromJson(doc.object());
    sensor.setId(sensorId);

    if (DbManager::instance().updateSensor(sensor)) {
        QJsonObject json;
        json["status"] = "success";
        json["message"] = "Sensor updated";
        json["sensor"] = sensor.toJson();

        response.setStatus(200, "OK");
        response.setBody(QJsonDocument(json).toJson());
    } else {
        response.setStatus(500, "Internal Server Error");
        response.setBody("{\"error\":\"Failed to update sensor\"}");
    }
}

void SensorController::remove(const HttpRequest &request, HttpResponse &response) {
    QString sensorId = request.getParam("id");
    if (sensorId.isEmpty()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Sensor ID required\"}");
        return;
    }

    if (DbManager::instance().deleteSensor(sensorId)) {
        QJsonObject json;
        json["status"] = "success";
        json["message"] = "Sensor deleted";

        response.setStatus(200, "OK");
        response.setBody(QJsonDocument(json).toJson());
    } else {
        response.setStatus(500, "Internal Server Error");
        response.setBody("{\"error\":\"Failed to delete sensor\"}");
    }
}

void SensorController::getHistory(const HttpRequest &request, HttpResponse &response) {
    QString sensorId = request.getParam("id");
    if (sensorId.isEmpty()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Sensor ID required\"}");
        return;
    }

    int count = request.getParam("count").toInt();
    if (count <= 0) count = 100;

    QList<SensorReading> readings = DbManager::instance().getHistory(sensorId, count);

    QJsonArray jsonArray;
    for (const SensorReading &reading : readings) {
        jsonArray.append(reading.toJson());
    }

    QJsonObject json;
    json["status"] = "success";
    json["readings"] = jsonArray;

    response.setStatus(200, "OK");
    response.setBody(QJsonDocument(json).toJson());
}
