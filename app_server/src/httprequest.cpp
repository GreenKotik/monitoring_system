#include "httprequest.h"
#include <QDebug>
#include <QUrl>

HttpRequest::HttpRequest() {}

HttpRequest::~HttpRequest() {}

bool HttpRequest::parse(const QByteArray &data) {
    QList<QByteArray> lines = data.split('\r');

    if (lines.isEmpty()) {
        return false;
    }

    // Парсим первую строку (метод, путь, версия)
    QString requestLine = QString::fromUtf8(lines[0]).trimmed();
    if (!parseRequestLine(requestLine)) {
        return false;
    }

    // Парсим заголовки
    int i = 1;
    for (; i < lines.size(); ++i) {
        QString line = QString::fromUtf8(lines[i]).trimmed();
        if (line.isEmpty()) {
            break;
        }

        int colonPos = line.indexOf(':');
        if (colonPos > 0) {
            QString key = line.left(colonPos).trimmed();
            QString value = line.mid(colonPos + 1).trimmed();
            m_headers[key] = value;
        }
    }

    // Парсим тело (оставшиеся строки)
    if (i < lines.size()) {
        QString body;
        for (int j = i + 1; j < lines.size(); ++j) {
            body += QString::fromUtf8(lines[j]);
        }
        m_body = body.toUtf8();
    }

    return true;
}

bool HttpRequest::parseRequestLine(const QString &line) {
    QStringList parts = line.split(' ');
    if (parts.size() < 3) {
        return false;
    }

    m_method = parts[0];

    QString path = parts[1];
    int queryPos = path.indexOf('?');
    if (queryPos > 0) {
        m_path = path.left(queryPos);
        m_queryString = path.mid(queryPos + 1);
        parseQueryParams(m_queryString);
    } else {
        m_path = path;
    }

    return true;
}

void HttpRequest::parseHeaders(const QList<QByteArray> &lines) {
    // Уже реализовано в parse()
}

void HttpRequest::parseBody(const QByteArray &data) {
    m_body = data;
}

void HttpRequest::parseQueryParams(const QString &query) {
    QStringList params = query.split('&');
    for (const QString &param : params) {
        QStringList keyValue = param.split('=');
        if (keyValue.size() == 2) {
            m_params[keyValue[0]] = QUrl::fromPercentEncoding(keyValue[1].toUtf8());
        }
    }
}

QString HttpRequest::method() const {
    return m_method;
}

QString HttpRequest::path() const {
    return m_path;
}

QByteArray HttpRequest::body() const {
    return m_body;
}

QString HttpRequest::getParam(const QString &name) const {
    if (m_params.contains(name)) {
        return m_params[name];
    }
    if (m_pathParams.contains(name)) {
        return m_pathParams[name];
    }
    return QString();
}

void HttpRequest::setParam(const QString &name, const QString &value) {
    m_params[name] = value;
}

void HttpRequest::setPathParams(const QMap<QString, QString> &params) {
    m_pathParams = params;
}

QString HttpRequest::header(const QString &name) const {
    return m_headers.value(name);
}

void HttpRequest::setHeader(const QString &name, const QString &value) {
    m_headers[name] = value;
}
