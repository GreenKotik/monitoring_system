#include <QCoreApplication>
#include <QDebug>
#include "httpserver.h"
#include "controllers/objectcontroller.h"
#include "controllers/sensorcontroller.h"
#include "controllers/authcontroller.h"
#include "controllers/mapcontroller.h"
#include "middleware/authmiddleware.h"
#include "middleware/corsmiddleware.h"
#include "core/database/dbmanager.h"
#include "core/utils/logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Logger::instance().init("app_server.log");
    Logger::instance().info("App Server started");

    if (!DbManager::instance().connect("localhost", 5432, "monitoring", "postgres", "password")) {
        Logger::instance().error("Failed to connect to database: " + DbManager::instance().lastError());
        return 1;
    }

    HttpServer server;

    // CORS
    CorsMiddleware::apply(server.router());

    // Публичные маршруты
    server.router()->addPost("/api/auth/login", AuthController::login);
    server.router()->addPost("/api/auth/logout", AuthController::logout);

    // Защищённые маршруты
    AuthMiddleware auth;

    // Объекты
    server.router()->addGet("/api/objects", auth.wrap(ObjectController::list));
    server.router()->addGet("/api/objects/tree", auth.wrap(ObjectController::getTree));
    server.router()->addGet("/api/objects/:id", auth.wrap(ObjectController::get));
    server.router()->addPost("/api/objects", auth.wrap(ObjectController::create));
    server.router()->addPut("/api/objects/:id", auth.wrap(ObjectController::update));
    server.router()->addDelete("/api/objects/:id", auth.wrap(ObjectController::remove));

    // Датчики
    server.router()->addGet("/api/sensors", auth.wrap(SensorController::list));
    server.router()->addGet("/api/sensors/:id", auth.wrap(SensorController::get));
    server.router()->addGet("/api/sensors/:id/history", auth.wrap(SensorController::history));
    server.router()->addPost("/api/sensors/:id/reading", auth.wrap(SensorController::addReading));
    server.router()->addPost("/api/sensors", auth.wrap(SensorController::create));
    server.router()->addPut("/api/sensors/:id", auth.wrap(SensorController::update));
    server.router()->addDelete("/api/sensors/:id", auth.wrap(SensorController::remove));

    // Типы
    server.router()->addGet("/api/object-types", auth.wrap(ObjectController::types));
    server.router()->addGet("/api/sensor-types", auth.wrap(SensorController::types));

    // Карты и схемы
    server.router()->addGet("/api/maps/:id", auth.wrap(MapController::getMap));
    server.router()->addGet("/api/schemes/:id", auth.wrap(MapController::getScheme));

    quint16 port = 3000;
    if (!server.start(port)) {
        Logger::instance().error("Failed to start HTTP server on port " + QString::number(port));
        return 1;
    }

    Logger::instance().info("HTTP Server started on port " + QString::number(port));

    return app.exec();
}