#include "pasoserver.h"

#include "commdata.h"

#include <QDataStream>
#include <QSqlError>
#include <QTcpSocket>

using namespace paso::comm;
using namespace paso::db;

namespace paso {
namespace server {

PasoServer::PasoServer(QObject *parent)
    : QObject(parent), mPort(6789), mTimeout(5000), mDatabaseName("paso") {
    mTcpServer = new QTcpServer(this);
    connect(mTcpServer, &QTcpServer::newConnection, this,
            &PasoServer::handleRequest);
}

bool PasoServer::loadConfiguration() {
    mDbName = "paso";
    mDbServer = "localhost";
    mDbUsername = "paso";
    mDbPassword = "paso";
    mDbPort = 5432;

    return true;
}

bool PasoServer::initDatabaseSystem() {
    auto db = QSqlDatabase::addDatabase("QPSQL", mDatabaseName);
    db.setDatabaseName(mDbName);
    db.setHostName(mDbServer);
    db.setUserName(mDbUsername);
    db.setPassword(mDbPassword);
    db.setPort(mDbPort);
    if (!db.open()) {
        qCritical() << db.lastError();
        qFatal(
            "Cannot establish database connection! Check your configuration.");
        return false;
    }
    mDbManager = std::make_shared<DBManager>(mDatabaseName);
    return true;
}

bool PasoServer::startServer() {
    if (!mTcpServer->listen(QHostAddress::Any, mPort)) {
        qCritical() << mTcpServer->errorString();
        qFatal("Cannot listen on TCP port!");
        return false;
    }
    return true;
}

void PasoServer::handleRequest() {
    auto clientSocket = mTcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket,
            &QObject::deleteLater);

    if (!clientSocket->waitForReadyRead(mTimeout)) {
        clientSocket->disconnectFromHost();
        return;
    }

    quint16 blockSize;
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_5_5);
    in >> blockSize;

    while (clientSocket->bytesAvailable() < blockSize) {
        if (!clientSocket->waitForReadyRead(mTimeout)) {
            clientSocket->disconnectFromHost();
            return;
        }
    }
    QString request;
    in >> request;
    Base base(request);
    if (base.operation() == Operation::LOGIN) {
        handleLoginRequest(clientSocket, request);
    } else {
        clientSocket->disconnectFromHost();
    }
}

void PasoServer::handleLoginRequest(QTcpSocket *clientSocket,
                                    const QString &requestString) {
    QString responseData;
    LoginRequest loginRequest;
    loginRequest.fromJsonString(requestString);
    QSqlError error;
    auto sysUser = mDbManager->getSystemUser(loginRequest.username(), error);
    if (sysUser && sysUser->password() == loginRequest.password()) {
        LoginResponse response(*sysUser, mDbName, mDbServer, mDbUsername,
                               mDbPassword, mDbPort);
        responseData = response.toJsonString();
    } else {
        LoginResponse response;
        responseData = response.toJsonString();
    }
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);
    out << (quint16)0;
    out << responseData;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    clientSocket->write(block);
    clientSocket->disconnectFromHost();
}
}
}
