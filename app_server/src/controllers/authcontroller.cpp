#include "controllers/authcontroller.h"
#include "database/dbmanager.h"
#include "models/user.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QUuid>

void AuthController::login(const HttpRequest &request, HttpResponse &response) {
    QJsonDocument doc = QJsonDocument::fromJson(request.body());
    QJsonObject data = doc.object();

    QString username = data["username"].toString();
    QString password = data["password"].toString();

    if (username.isEmpty() || password.isEmpty()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Username and password required\"}");
        return;
    }

    // TODO: Проверка пользователя в БД
    // Для упрощения считаем любого пользователя валидным
    QJsonObject json;
    json["status"] = "success";
    json["token"] = "jwt-token";
    json["user"] = QJsonObject{{"username", username}};

    response.setStatus(200, "OK");
    response.setBody(QJsonDocument(json).toJson());
}

void AuthController::signup(const HttpRequest &request, HttpResponse &response) {
    QJsonDocument doc = QJsonDocument::fromJson(request.body());
    QJsonObject data = doc.object();

    QString username = data["username"].toString();
    QString password = data["password"].toString();
    QString email = data["email"].toString();

    if (username.isEmpty() || password.isEmpty()) {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"error\":\"Username and password required\"}");
        return;
    }

    // TODO: Создание пользователя в БД
    QJsonObject json;
    json["status"] = "success";
    json["message"] = "User created successfully";

    response.setStatus(201, "Created");
    response.setBody(QJsonDocument(json).toJson());
}

void AuthController::logout(const HttpRequest &request, HttpResponse &response) {
    response.setStatus(200, "OK");
    response.setBody("{\"message\":\"Logged out\"}");
}

void AuthController::refresh(const HttpRequest &request, HttpResponse &response) {
    QJsonObject json;
    json["status"] = "success";
    json["token"] = "new-jwt-token";

    response.setStatus(200, "OK");
    response.setBody(QJsonDocument(json).toJson());
}
