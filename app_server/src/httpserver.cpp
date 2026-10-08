#include "httpserver.h"
#include <QDebug>

HttpServer::HttpServer(QObject *parent)
    : QTcpServer(parent)
    , m_router(nullptr)
{
}

HttpServer::~HttpServer() {
    stop();
}

bool HttpServer::start(int port) {
    if (!listen(QHostAddress::Any, port)) {
        qCritical() << "Failed to start HTTP server on port" << port << ":" << errorString();
        return false;
    }
    qDebug() << "HTTP Server started on port" << port;
    return true;
}

void HttpServer::stop() {
    close();
    qDebug() << "HTTP Server stopped";
}

void HttpServer::setRouter(HttpRouter *router) {
    m_router = router;
}

void HttpServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &HttpServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &HttpServer::onDisconnected);

    m_buffers[socket] = QByteArray();
}

void HttpServer::onReadyRead() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    m_buffers[socket].append(socket->readAll());

    QByteArray &buffer = m_buffers[socket];
    if (buffer.contains("\r\n\r\n") || buffer.contains("\n\n")) {
        processRequest(socket, buffer);
        buffer.clear();
    }
}

void HttpServer::onDisconnected() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    m_buffers.remove(socket);
    socket->deleteLater();
}

void HttpServer::processRequest(QTcpSocket *socket, const QByteArray &data) {
    HttpRequest request;
    HttpResponse response;

    if (!request.parse(data)) {
        response.setStatus(400, "Bad Request");
        response.setBody(QByteArray("{\"error\":\"Invalid HTTP request\"}"));
        socket->write(response.toByteArray());
        socket->flush();
        return;
    }

    if (m_router) {
        m_router->route(request, response);
    } else {
        response.setStatus(404, "Not Found");
        response.setBody(QByteArray("{\"error\":\"No router configured\"}"));
    }

    socket->write(response.toByteArray());
    socket->flush();
}
