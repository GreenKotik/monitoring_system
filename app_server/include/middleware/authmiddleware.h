#pragma once

#include <QString>
#include "httprequest.h"
#include "httpresponse.h"

class AuthMiddleware
{
public:
    static bool check(const HttpRequest &request, HttpResponse &response);
    static bool isAuthenticated(const HttpRequest &request);
    static QString getUsername(const HttpRequest &request);
};
