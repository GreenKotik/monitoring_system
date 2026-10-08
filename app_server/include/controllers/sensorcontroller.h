#pragma once

#include <QObject>
#include "httprequest.h"
#include "httpresponse.h"

class SensorController : public QObject
{
    Q_OBJECT

public:
    static void list(const HttpRequest &request, HttpResponse &response);
    static void get(const HttpRequest &request, HttpResponse &response);
    static void create(const HttpRequest &request, HttpResponse &response);
    static void update(const HttpRequest &request, HttpResponse &response);
    static void remove(const HttpRequest &request, HttpResponse &response);
    static void getHistory(const HttpRequest &request, HttpResponse &response);
};
