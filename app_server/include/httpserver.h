#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "httprequest.h"
#include "httpresponse.h"
#include "httprouter.h"

class HttpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit HttpServer(QObject *parent = nullptr);
    ~HttpServer();

    bool start(int port);
    void stop();

    void setRouter(HttpRouter *router);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    void processRequest(QTcpSocket *socket, const QByteArray &data);

    HttpRouter *m_router;
    QMap<QTcpSocket*, QByteArray> m_buffers;
};
