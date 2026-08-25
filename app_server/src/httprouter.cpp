#include "httprouter.h"
#include <QDebug>
#include <QRegularExpression>

HttpRouter::HttpRouter(QObject *parent)
    : QObject(parent)
{
    m_notFoundHandler = [](const HttpRequest &req, HttpResponse &res) {
        res.setStatus(HttpResponse::NOT_FOUND);
        res.setJson(R"({"error": "Not Found", "path": ")" + req.path() + R"("})");
    };
}

void HttpRouter::addRoute(const QString &method, const QString &path, Handler handler)
{
    Route route;
    route.method = method.toUpper();
    route.path = path;
    route.handler = handler;
    m_routes.append(route);
    qDebug() << "Route added:" << route.method << route.path;
}

void HttpRouter::addGet(const QString &path, Handler handler)
{
    addRoute("GET", path, handler);
}

void HttpRouter::addPost(const QString &path, Handler handler)
{
    addRoute("POST", path, handler);
}

void HttpRouter::addPut(const QString &path, Handler handler)
{
    addRoute("PUT", path, handler);
}

void HttpRouter::addDelete(const QString &path, Handler handler)
{
    addRoute("DELETE", path, handler);
}

bool HttpRouter::route(const HttpRequest &request, HttpResponse &response)
{
    QString method = request.methodString();
    QString path = request.path();

    qDebug() << "Routing:" << method << path;

    for (const Route &route : m_routes) {
        if (route.method != method) continue;

        QMap<QString, QString> params;
        if (matchRoute(route.path, path, params)) {
            qDebug() << "Matched route:" << route.path;
            route.handler(request, response);
            return true;
        }
    }

    qDebug() << "No route found for:" << method << path;
    m_notFoundHandler(request, response);
    return false;
}

void HttpRouter::setNotFoundHandler(Handler handler)
{
    m_notFoundHandler = handler;
}

bool HttpRouter::matchRoute(const QString &routePath, const QString &requestPath, QMap<QString, QString> &params)
{
    if (routePath == requestPath) {
        return true;
    }

    QStringList routeParts = routePath.split('/');
    QStringList requestParts = requestPath.split('/');

    if (routeParts.size() != requestParts.size()) {
        return false;
    }

    for (int i = 0; i < routeParts.size(); ++i) {
        if (routeParts[i].startsWith(':')) {
            QString paramName = routeParts[i].mid(1);
            params[paramName] = requestParts[i];
        } else if (routeParts[i] != requestParts[i]) {
            return false;
        }
    }

    return true;
}