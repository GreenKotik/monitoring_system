#pragma once

#include "httprequest.h"
#include "httpresponse.h"

class CORS
{
public:
    static void apply(HttpResponse &response);
    static bool handlePreflight(const HttpRequest &request, HttpResponse &response);
};
