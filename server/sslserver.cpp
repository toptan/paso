#include "sslserver.h"

#include <QSslCipher>
#include <QSslConfiguration>
#include <QSslSocket>

SslServer::SslServer(std::shared_ptr<QSslCertificate> certificate,
                     std::shared_ptr<QSslKey> key, QObject *parent)
    : QTcpServer(parent), mCertificate(certificate), mKey(key) {}

SslServer::~SslServer() {}

void SslServer::incomingConnection(qintptr socketDescriptor) {
    QSslSocket *serverSocket = new QSslSocket;
    serverSocket->ignoreSslErrors();
    if (serverSocket->setSocketDescriptor(socketDescriptor)) {
        connect(serverSocket, &QAbstractSocket::disconnected, serverSocket,
                &QObject::deleteLater);
        serverSocket->setPrivateKey(*mKey);
        serverSocket->setLocalCertificate(*mCertificate);
        serverSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
        addPendingConnection(serverSocket);
        serverSocket->startServerEncryption();
        if (!serverSocket->waitForEncrypted()) {
            serverSocket->disconnectFromHost();
            delete serverSocket;
        }
    } else {
        delete serverSocket;
    }
}
