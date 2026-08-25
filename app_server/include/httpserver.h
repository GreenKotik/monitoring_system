#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QThreadPool>
#include "httprouter.h"
#include "httprequest.h"
#include "httpresponse.h"

class HttpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit HttpServer(QObject *parent = nullptr);
    ~HttpServer();

    bool start(quint16 port);
    void stop();

    HttpRouter* router() { return &m_router; }

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    void processRequest(QTcpSocket *socket, const QByteArray &data);

    HttpRouter m_router;
    QMap<QTcpSocket*, QByteArray> m_buffers;
};

#endif // HTTPSERVER_H