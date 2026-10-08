#include "middleware/authmiddleware.h"
#include "controllers/authcontroller.h"

bool AuthMiddleware::check(const HttpRequest &request, HttpResponse &response) {
    // Проверяем наличие заголовка Authorization
    QString authHeader = request.header("Authorization");
    if (authHeader.isEmpty()) {
        response.setStatus(401, "Unauthorized");
        response.setBody("{\"error\": \"Authorization required\"}");
        return false;
    }

    // Проверяем токен (упрощенно)
    if (!authHeader.startsWith("Bearer ")) {
        response.setStatus(401, "Unauthorized");
        response.setBody("{\"error\": \"Invalid token format\"}");
        return false;
    }

    QString token = authHeader.mid(7);
    if (token.isEmpty()) {
        response.setStatus(401, "Unauthorized");
        response.setBody("{\"error\": \"Empty token\"}");
        return false;
    }

    // TODO: Реальная проверка JWT токена
    // Для упрощения считаем, что любой непустой токен валидный
    return true;
}

bool AuthMiddleware::isAuthenticated(const HttpRequest &request) {
    QString authHeader = request.header("Authorization");
    if (authHeader.isEmpty()) {
        return false;
    }
    if (!authHeader.startsWith("Bearer ")) {
        return false;
    }
    QString token = authHeader.mid(7);
    return !token.isEmpty();
}

QString AuthMiddleware::getUsername(const HttpRequest &request) {
    // TODO: Извлечение username из JWT токена
    return "user";
}
