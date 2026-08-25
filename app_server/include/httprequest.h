#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QString>
#include <QMap>
#include <QByteArray>
#include <QUrl>

class HttpRequest
{
public:
    enum Method {
        GET,
        POST,
        PUT,
        DELETE,
        PATCH,
        OPTIONS,
        HEAD,
        UNKNOWN
    };

    HttpRequest();
    ~HttpRequest();

    bool parse(const QByteArray &data);

    Method method() const { return m_method; }
    QString methodString() const;
    QString path() const { return m_path; }
    QString version() const { return m_version; }
    QMap<QString, QString> headers() const { return m_headers; }
    QByteArray body() const { return m_body; }
    QMap<QString, QString> queryParams() const { return m_queryParams; }
    QMap<QString, QString> pathParams() const { return m_pathParams; }

    QString header(const QString &name) const;
    QString queryParam(const QString &name) const;
    QString pathParam(const QString &name) const;

    void setPathParams(const QMap<QString, QString> &params) { m_pathParams = params; }

private:
    Method m_method;
    QString m_path;
    QString m_version;
    QMap<QString, QString> m_headers;
    QByteArray m_body;
    QMap<QString, QString> m_queryParams;
    QMap<QString, QString> m_pathParams;

    Method stringToMethod(const QString &str);
    void parseQueryParams(const QString &query);
};

#endif // HTTPREQUEST_H