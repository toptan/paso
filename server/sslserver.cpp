#include "sslserver.h"

#include <QSslSocket>
#include <QSslConfiguration>
#include <QSslCipher>

SslServer::SslServer(QObject *parent) : QTcpServer(parent) {
}

SslServer::~SslServer() {}

void SslServer::incomingConnection(qintptr socketDescriptor) {
    qDebug() << QSslConfiguration::supportedCiphers();
    QSslSocket *serverSocket = new QSslSocket;
    connect(serverSocket, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors),
        [=](const QList<QSslError> &errors){
        qDebug() << errors;
    });
    serverSocket->ignoreSslErrors();
    if (serverSocket->setSocketDescriptor(socketDescriptor)) {
        addPendingConnection(serverSocket);
//        connect(serverSocket, &QSslSocket::encrypted, this, &SslServer::ready);
        serverSocket->startServerEncryption();
        if(!serverSocket->waitForEncrypted()) {
            qCritical() << "FAK:" << serverSocket->errorString() <<
            serverSocket->sslErrors();
        }
    } else {
        delete serverSocket;
    }
}
