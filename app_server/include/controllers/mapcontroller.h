#pragma once

#include <QObject>
#include "httprequest.h"
#include "httpresponse.h"

class MapController : public QObject
{
    Q_OBJECT

public:
    static void getMap(const HttpRequest &request, HttpResponse &response);
    static void getScheme(const HttpRequest &request, HttpResponse &response);
};
