#include "middleware/corsmiddleware.h"

void CORS::apply(HttpResponse &response) {
    // Разрешаем запросы с любых доменов (для разработки)
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    response.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    response.setHeader("Access-Control-Max-Age", "86400");
}

bool CORS::handlePreflight(const HttpRequest &request, HttpResponse &response) {
    // Проверяем, является ли запрос OPTIONS (preflight)
    if (request.method().toUpper() == "OPTIONS") {  // ИСПРАВЛЕНО: метод() вместо methodString()
        apply(response);
        response.setStatus(200, "OK");  // ИСПРАВЛЕНО: setStatus(200, "OK")
        response.setBody(QByteArray());  // Пустое тело для preflight
        return true;
    }
    return false;
}
