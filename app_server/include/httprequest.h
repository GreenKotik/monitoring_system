#pragma once

#include <QString>
#include <QByteArray>
#include <QMap>
#pragma once

#include <QString>
#include <QByteArray>
#include <QMap>
#include <QList>

class HttpRequest
{
public:
    HttpRequest();
    ~HttpRequest();

    bool parse(const QByteArray &data);

    QString method() const;
    QString path() const;
    QByteArray body() const;

    QString getParam(const QString &name) const;
    void setParam(const QString &name, const QString &value);

    void setPathParams(const QMap<QString, QString> &params);

    QString header(const QString &name) const;
    void setHeader(const QString &name, const QString &value);

private:
    bool parseRequestLine(const QString &line);
    void parseHeaders(const QList<QByteArray> &lines);
    void parseBody(const QByteArray &data);
    void parseQueryParams(const QString &query);

    QString m_method;
    QString m_path;
    QString m_queryString;
    QByteArray m_body;
    QMap<QString, QString> m_headers;
    QMap<QString, QString> m_params;
    QMap<QString, QString> m_pathParams;
};
