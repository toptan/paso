#include "pasoserver.h"

#include "commdata.h"

#include <QDataStream>
#include <QFile>
#include <QSettings>
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

bool PasoServer::loadConfiguration(const QString &configFile) {
    if (!QFile::exists(configFile)) {
        qCritical()
            << "The configuration file" << configFile
            << "does not exist or is not readable. Server will not start!";
        return false;
    }
    qInfo() << "Reading PaSo server configuration from" << configFile;
    QSettings settings(configFile, QSettings::IniFormat);
    settings.beginGroup("server");
    mPort = settings.value("port", 6789).toInt();
    mTimeout = settings.value("timeout", 5000).toInt();
    settings.endGroup();

    settings.beginGroup("database");
    if (!settings.contains("database")) {
        qCritical() << "No database name present in configuration file. Server "
                       "will not start!";
        return false;
    }
    if (!settings.contains("server")) {
        qCritical() << "No database server name present in configuration file. "
                       "Server will not start!";
        return false;
    }
    if (!settings.contains("port")) {
        qCritical() << "No database server port present in configuration file. "
                       "Server will not start!";
        return false;
    }
    if (!settings.contains("username")) {
        qCritical() << "No database username present in configuration file. "
                       "Server will not start!";
        return false;
    }
    if (!settings.contains("password")) {
        qCritical() << "No database password present in configuration file. "
                       "Server will not start!";
        return false;
    }

    mDbName = settings.value("database").toString();
    mDbServer = settings.value("server").toString();
    mDbPort = settings.value("port").toInt();
    mDbUsername = settings.value("username").toString();
    mDbPassword = settings.value("password").toString();
    settings.endGroup();

    qInfo() << "Server will start on port" << mPort;
    qInfo() << "Operation timeout set to" << mTimeout << "milliseconds.";
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
        qCritical() << "Cannot establish database connection! Check your "
                       "configuration.";
        qCritical() << "Database error code:" << db.lastError().number();
        qCritical() << "Database error text:" << db.lastError().text();
        return false;
    }
    mDbManager = std::make_shared<DBManager>(mDatabaseName);
    qInfo() << "Database connection successfully establised.";
    return true;
}

bool PasoServer::startServer() {
    if (!mTcpServer->listen(QHostAddress::Any, mPort)) {
        qCritical() << "Cannot start server on port " << mPort;
        qCritical() << mTcpServer->errorString();
        return false;
    }
    qInfo() << "Server started and is listening for connections.";
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
        LoginResponse response(*sysUser, "QPSQL", mDbName, mDbServer,
                               mDbUsername, mDbPassword, mDbPort);
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
