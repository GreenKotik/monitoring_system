#include "httprouter.h"
#include <QDebug>

HttpRouter::HttpRouter(QObject *parent)
    : QObject(parent)
{
}

HttpRouter::~HttpRouter() {}

void HttpRouter::addRoute(const QString &method, const QString &path, Handler handler) {
    Route route;
    route.method = method.toUpper();
    route.path = path;
    route.handler = handler;
    m_routes.append(route);
}

bool HttpRouter::route(const HttpRequest &request, HttpResponse &response) {
    QString method = request.method().toUpper();
    QString path = request.path();

    for (const Route &route : m_routes) {
        if (route.method != method) continue;

        QStringList routeParts = route.path.split('/');
        QStringList pathParts = path.split('/');

        if (routeParts.size() == pathParts.size()) {
            bool match = true;
            QMap<QString, QString> params;

            for (int i = 0; i < routeParts.size(); ++i) {
                if (routeParts[i].startsWith(':')) {
                    params[routeParts[i].mid(1)] = pathParts[i];
                } else if (routeParts[i] != pathParts[i]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                HttpRequest requestWithParams = request;
                requestWithParams.setPathParams(params);
                route.handler(requestWithParams, response);
                return true;
            }
        }
    }

    response.setStatus(404, "Not Found");
    response.setBody(QByteArray("{\"error\":\"Route not found\"}"));
    return false;
}
