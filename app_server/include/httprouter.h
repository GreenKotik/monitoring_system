#pragma once

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
    using Handler = std::function<void(const HttpRequest&, HttpResponse&)>;

    explicit HttpRouter(QObject *parent = nullptr);
    ~HttpRouter();

    void addRoute(const QString &method, const QString &path, Handler handler);
    bool route(const HttpRequest &request, HttpResponse &response);

private:
    struct Route {
        QString method;
        QString path;
        Handler handler;
    };

    QList<Route> m_routes;
};
