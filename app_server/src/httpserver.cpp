#include "httpserver.h"
#include <QDebug>

HttpServer::HttpServer(QObject *parent)
    : QTcpServer(parent)
{
    qDebug() << "HttpServer initialized";
}

HttpServer::~HttpServer()
{
    stop();
}

bool HttpServer::start(quint16 port)
{
    if (listen(QHostAddress::Any, port)) {
        qDebug() << "HttpServer started on port" << port;
        return true;
    } else {
        qDebug() << "Failed to start HttpServer on port" << port << errorString();
        return false;
    }
}

void HttpServer::stop()
{
    close();
    qDebug() << "HttpServer stopped";
}

void HttpServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &HttpServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &HttpServer::onDisconnected);

    m_buffers[socket] = QByteArray();

    qDebug() << "New connection from" << socket->peerAddress().toString() << ":" << socket->peerPort();
}

void HttpServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    m_buffers[socket].append(data);

    // Проверяем, полный ли запрос получен
    if (m_buffers[socket].contains("\r\n\r\n")) {
        processRequest(socket, m_buffers[socket]);
        m_buffers[socket].clear();
    }
}

void HttpServer::onDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    m_buffers.remove(socket);
    socket->deleteLater();
    qDebug() << "Connection closed";
}

void HttpServer::processRequest(QTcpSocket *socket, const QByteArray &data)
{
    HttpRequest request;
    if (!request.parse(data)) {
        HttpResponse response;
        response.setStatus(HttpResponse::BAD_REQUEST);
        response.setBody("Bad Request");
        socket->write(response.toByteArray());
        socket->flush();
        return;
    }

    HttpResponse response;
    m_router.route(request, response);

    socket->write(response.toByteArray());
    socket->flush();
}