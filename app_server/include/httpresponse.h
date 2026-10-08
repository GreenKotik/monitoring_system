#pragma once
#pragma once

#pragma once

#include <QString>
#include <QByteArray>
#include <QMap>

class HttpResponse
{
public:
    HttpResponse();
    ~HttpResponse();

    void setStatus(int statusCode, const QString &reason = "OK");
    int statusCode() const;
    QString reason() const;

    // Оставляем только один метод с QByteArray
    void setBody(const QByteArray &body);
    QByteArray body() const;

    void setHeader(const QString &name, const QString &value);
    QString header(const QString &name) const;

    QByteArray toByteArray() const;

private:
    int m_statusCode;
    QString m_reason;
    QByteArray m_body;
    QMap<QString, QString> m_headers;
};
