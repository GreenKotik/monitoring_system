#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QByteArray>
#include <QMap>
#include <QString>

class HttpResponse
{
public:
    enum StatusCode {
        OK = 200,
        CREATED = 201,
        NO_CONTENT = 204,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        INTERNAL_SERVER_ERROR = 500
    };

    HttpResponse();

    void setStatus(StatusCode code);
    void setHeader(const QString &name, const QString &value);
    void setBody(const QByteArray &body);
    void setBody(const QString &body);
    void setJson(const QByteArray &json);
    void setJson(const QString &json);

    QByteArray toByteArray() const;

private:
    StatusCode m_statusCode;
    QMap<QString, QString> m_headers;
    QByteArray m_body;

    QString statusText(StatusCode code) const;
};

#endif // HTTPRESPONSE_H