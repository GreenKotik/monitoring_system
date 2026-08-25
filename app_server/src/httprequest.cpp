#include "httprequest.h"
#include <QDebug>

HttpRequest::HttpRequest()
    : m_method(UNKNOWN)
{
}

HttpRequest::~HttpRequest()
{
}

bool HttpRequest::parse(const QByteArray &data)
{
    QList<QByteArray> lines = data.split('\r\n');
    if (lines.isEmpty()) return false;

    QList<QByteArray> firstLine = lines[0].split(' ');
    if (firstLine.size() < 3) return false;

    m_method = stringToMethod(QString::fromUtf8(firstLine[0]));
    if (m_method == UNKNOWN) return false;

    QString pathWithQuery = QString::fromUtf8(firstLine[1]);
    int queryStart = pathWithQuery.indexOf('?');
    if (queryStart != -1) {
        m_path = pathWithQuery.left(queryStart);
        parseQueryParams(pathWithQuery.mid(queryStart + 1));
    } else {
        m_path = pathWithQuery;
    }

    m_version = QString::fromUtf8(firstLine[2]);

    int i = 1;
    while (i < lines.size()) {
        QByteArray line = lines[i];
        if (line.isEmpty()) break;

        int colonPos = line.indexOf(':');
        if (colonPos != -1) {
            QString key = QString::fromUtf8(line.left(colonPos)).trimmed();
            QString value = QString::fromUtf8(line.mid(colonPos + 1)).trimmed();
            m_headers[key] = value;
        }
        i++;
    }

    i++;
    if (i < lines.size()) {
        m_body = lines[i];
    }

    return true;
}

QString HttpRequest::methodString() const
{
    switch (m_method) {
        case GET: return "GET";
        case POST: return "POST";
        case PUT: return "PUT";
        case DELETE: return "DELETE";
        case PATCH: return "PATCH";
        case OPTIONS: return "OPTIONS";
        case HEAD: return "HEAD";
        default: return "UNKNOWN";
    }
}

HttpRequest::Method HttpRequest::stringToMethod(const QString &str)
{
    if (str == "GET") return GET;
    if (str == "POST") return POST;
    if (str == "PUT") return PUT;
    if (str == "DELETE") return DELETE;
    if (str == "PATCH") return PATCH;
    if (str == "OPTIONS") return OPTIONS;
    if (str == "HEAD") return HEAD;
    return UNKNOWN;
}

void HttpRequest::parseQueryParams(const QString &query)
{
    QList<QString> pairs = query.split('&');
    for (const QString &pair : pairs) {
        int eqPos = pair.indexOf('=');
        if (eqPos != -1) {
            QString key = pair.left(eqPos);
            QString value = pair.mid(eqPos + 1);
            m_queryParams[key] = QUrl::fromPercentEncoding(value.toUtf8());
        }
    }
}

QString HttpRequest::header(const QString &name) const
{
    return m_headers.value(name, QString());
}

QString HttpRequest::queryParam(const QString &name) const
{
    return m_queryParams.value(name, QString());
}

QString HttpRequest::pathParam(const QString &name) const
{
    return m_pathParams.value(name, QString());
}