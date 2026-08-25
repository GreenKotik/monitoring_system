#ifndef HTTPROUTER_H
#define HTTPROUTER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <functional>
#include "httprequest.h"
#include "httpresponse.h"

class HttpRouter : public QObject
{
    Q_OBJECT

public:
    using Handler = std::function<void(const HttpRequest &, HttpResponse &)>;

    HttpRouter(QObject *parent = nullptr);

    void addRoute(const QString &method, const QString &path, Handler handler);
    void addGet(const QString &path, Handler handler);
    void addPost(const QString &path, Handler handler);
    void addPut(const QString &path, Handler handler);
    void addDelete(const QString &path, Handler handler);

    bool route(const HttpRequest &request, HttpResponse &response);

    void setNotFoundHandler(Handler handler);

private:
    struct Route {
        QString method;
        QString path;
        Handler handler;
    };

    QList<Route> m_routes;
    Handler m_notFoundHandler;

    bool matchRoute(const QString &routePath, const QString &requestPath, QMap<QString, QString> &params);
};

#endif // HTTPROUTER_H