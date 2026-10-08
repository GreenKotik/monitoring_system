#pragma once

#include <QObject>
#include "httprequest.h"
#include "httpresponse.h"

class AuthController : public QObject
{
    Q_OBJECT

public:
    static void login(const HttpRequest &request, HttpResponse &response);
    static void signup(const HttpRequest &request, HttpResponse &response);
    static void logout(const HttpRequest &request, HttpResponse &response);
    static void refresh(const HttpRequest &request, HttpResponse &response);
};
