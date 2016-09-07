#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QSslSocket>

class SslServer : public QTcpServer {
public:
    explicit SslServer(QObject *parent = nullptr);
    virtual ~SslServer();

protected:
    virtual void incomingConnection(qintptr socketDescriptor) override;

private:
    QSslSocket mSocket;

};

#endif // SSLSERVER_H
