#include "httpresponse.h"
#include <QDateTime>

HttpResponse::HttpResponse()
    : m_statusCode(200)
    , m_reason("OK")
{
}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatus(int statusCode, const QString &reason) {
    m_statusCode = statusCode;
    if (!reason.isEmpty()) {
        m_reason = reason;
    }
}

int HttpResponse::statusCode() const {
    return m_statusCode;
}

QString HttpResponse::reason() const {
    return m_reason;
}

void HttpResponse::setBody(const QByteArray &body) {
    m_body = body;
    setHeader("Content-Length", QString::number(body.size()));
}

QByteArray HttpResponse::body() const {
    return m_body;
}

void HttpResponse::setHeader(const QString &name, const QString &value) {
    m_headers[name] = value;
}

QString HttpResponse::header(const QString &name) const {
    return m_headers.value(name);
}

QByteArray HttpResponse::toByteArray() const {
    QString response;
    response += QString("HTTP/1.1 %1 %2\r\n")
                .arg(m_statusCode)
                .arg(m_reason);

    for (auto it = m_headers.begin(); it != m_headers.end(); ++it) {
        response += it.key() + ": " + it.value() + "\r\n";
    }

    if (!m_headers.contains("Content-Type")) {
        response += "Content-Type: application/json\r\n";
    }
    if (!m_headers.contains("Server")) {
        response += "Server: AppServer/1.0\r\n";
    }
    if (!m_headers.contains("Date")) {
        response += "Date: " + QDateTime::currentDateTimeUtc().toString("ddd, dd MMM yyyy HH:mm:ss") + " GMT\r\n";
    }

    response += "\r\n";
    response += m_body;

    return response.toUtf8();
}
