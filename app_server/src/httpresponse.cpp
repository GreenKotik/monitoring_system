#include "httpresponse.h"
#include <QTextStream>
#include <QDebug>

HttpResponse::HttpResponse()
    : m_statusCode(OK)
{
    setHeader("Content-Type", "text/plain");
    setHeader("Server", "QtHttpServer/1.0");
}

void HttpResponse::setStatus(StatusCode code)
{
    m_statusCode = code;
}

void HttpResponse::setHeader(const QString &name, const QString &value)
{
    m_headers[name] = value;
}

void HttpResponse::setBody(const QByteArray &body)
{
    m_body = body;
    setHeader("Content-Length", QString::number(m_body.size()));
}

void HttpResponse::setBody(const QString &body)
{
    setBody(body.toUtf8());
}

void HttpResponse::setJson(const QByteArray &json)
{
    setHeader("Content-Type", "application/json");
    setBody(json);
}

void HttpResponse::setJson(const QString &json)
{
    setJson(json.toUtf8());
}

QByteArray HttpResponse::toByteArray() const
{
    QByteArray result;

    result += "HTTP/1.1 ";
    result += QByteArray::number(m_statusCode);
    result += " ";
    result += statusText(m_statusCode).toUtf8();
    result += "\r\n";

    for (auto it = m_headers.begin(); it != m_headers.end(); ++it) {
        result += it.key().toUtf8();
        result += ": ";
        result += it.value().toUtf8();
        result += "\r\n";
    }

    result += "\r\n";
    result += m_body;

    return result;
}

QString HttpResponse::statusText(StatusCode code) const
{
    switch (code) {
        case OK: return "OK";
        case CREATED: return "Created";
        case NO_CONTENT: return "No Content";
        case BAD_REQUEST: return "Bad Request";
        case UNAUTHORIZED: return "Unauthorized";
        case FORBIDDEN: return "Forbidden";
        case NOT_FOUND: return "Not Found";
        case METHOD_NOT_ALLOWED: return "Method Not Allowed";
        case INTERNAL_SERVER_ERROR: return "Internal Server Error";
        default: return "Unknown";
    }
}