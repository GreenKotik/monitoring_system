#ifndef OBJECTCONTROLLER_H
#define OBJECTCONTROLLER_H

#include <QJsonObject>
#include <QJsonArray>
#include "../httprequest.h"
#include "../httpresponse.h"
#include "core/database/dbmanager.h"
#include "core/models/object.h"
#include "core/utils/logger.h"

class ObjectController
{
public:
    static void list(const HttpRequest &req, HttpResponse &res);
    static void get(const HttpRequest &req, HttpResponse &res);
    static void create(const HttpRequest &req, HttpResponse &res);
    static void update(const HttpRequest &req, HttpResponse &res);
    static void remove(const HttpRequest &req, HttpResponse &res);
    static void getTree(const HttpRequest &req, HttpResponse &res);
    static void types(const HttpRequest &req, HttpResponse &res);
};

#endif // OBJECTCONTROLLER_H