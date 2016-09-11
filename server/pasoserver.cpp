#include "pasoserver.h"

#include "commaccess.h"
#include "commdata.h"
#include "commping.h"
#include "commregister.h"

#include <QDataStream>
#include <QFile>
#include <QSettings>
#include <QSqlError>
#include <QSslSocket>
#include <QTimer>

using namespace paso::comm;
using namespace paso::db;

namespace paso {
namespace server {

PasoServer::PasoServer(QObject *parent)
    : QObject(parent), mTcpServer(nullptr), mPort(6789), mTimeout(5000),
      mControllerCheckPeriod(300000), mDatabaseName("paso") {}

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
    mControllerCheckPeriod =
        settings.value("controllerCheckPeriod", 30).toInt() * 1000;

    if (!settings.contains("keyFile")) {
        qCritical() << "No server private key defined in configuration file. "
                       "The server will not start.";
        return false;
    }

    if (!settings.contains("certFile")) {
        qCritical() << "No server certificate defined in configuration file. "
                       "The server will not start.";
        return false;
    }

    mKeyFile = settings.value("keyFile").toString();
    mCertFile = settings.value("certFile").toString();

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
    qInfo() << "Controller check period set to" << mControllerCheckPeriod / 1000
            << "seconds.";
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
    QFile keyFile(mKeyFile);
    QFile certFile(mCertFile);

    if (!keyFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Could not open server key '" << mKeyFile
                    << "'. The server will not start.";
        return false;
    }

    if (!certFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Could not open server certificate '" << mCertFile
                    << "'. The server will not start.";
        return false;
    }

    auto key = std::make_shared<QSslKey>(&keyFile, QSsl::Rsa, QSsl::Pem,
                                         QSsl::PrivateKey);
    auto cert = std::make_shared<QSslCertificate>(&certFile);

    mTcpServer = new SslServer(cert, key, this);
    connect(mTcpServer, &QTcpServer::newConnection, this,
            &PasoServer::handleRequest);
    if (!mTcpServer->listen(QHostAddress::Any, mPort)) {
        qCritical() << "Cannot start server on port " << mPort;
        qCritical() << mTcpServer->errorString();
        return false;
    }
    qInfo() << "Server started and is listening for connections.";
    QTimer::singleShot(mControllerCheckPeriod, this,
                       &PasoServer::checkControllers);
    return true;
}

void PasoServer::handleRequest() {
    auto serverSocket = mTcpServer->nextPendingConnection();
    connect(serverSocket, &QTcpSocket::disconnected, serverSocket,
            &QObject::deleteLater);

    if (!serverSocket->waitForReadyRead(mTimeout)) {
        serverSocket->disconnectFromHost();
        qCritical() << serverSocket->errorString();
        return;
    }

    quint16 blockSize;
    QDataStream in(serverSocket);
    in.setVersion(QDataStream::Qt_5_5);
    in >> blockSize;

    while (serverSocket->bytesAvailable() < blockSize) {
        if (!serverSocket->waitForReadyRead(mTimeout)) {
            serverSocket->disconnectFromHost();
            return;
        }
    }
    QString request;
    in >> request;
    Base base(request);
    switch (base.operation()) {
    case Operation::LOGIN:
        handleLoginRequest(serverSocket, request);
        break;
    case Operation::REGISTER:
        handleRegisterRequest(serverSocket, request);
        break;
    case Operation::ACCESS:
        handleAccessRequest(serverSocket, request);
        break;
    default:
        qWarning() << "The controller" << base.roomId().toString()
                   << "requested invalid operation.";
        serverSocket->disconnectFromHost();
        break;
    }
}

void PasoServer::writeResponse(QTcpSocket *clientSocket,
                               const paso::comm::Base &response) const {

    QString responseData = response.toJsonString();
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

void PasoServer::handleLoginRequest(QTcpSocket *clientSocket,
                                    const QString &requestString) {
    LoginRequest request;
    request.fromJsonString(requestString);
    QSqlError error;
    auto sysUser = mDbManager->getSystemUser(request.username(), error);
    if (sysUser && sysUser->password() == request.password()) {
        LoginResponse response(*sysUser, "QPSQL", mDbName, mDbServer,
                               mDbUsername, mDbPassword, mDbPort);
        writeResponse(clientSocket, response);
    } else {
        LoginResponse response;
        writeResponse(clientSocket, response);
    }
}

void PasoServer::handleRegisterRequest(QTcpSocket *clientSocket,
                                       const QString &requestString) {
    QSqlError error;
    QString responseData;
    RegisterRequest request;
    request.fromJsonString(requestString);
    RegisterResponse response(request.roomId());
    response.setSuccess(false);
    auto room = mDbManager->getRoom(request.roomId(), error);
    if (error.type() != QSqlError::NoError) {
        qCritical() << "Problem loading room data:" << error;
        writeResponse(clientSocket, response);
        return;
    }
    if (!room) {
        qWarning() << "Unauthorized controller " << request.roomId().toString()
                   << "from" << clientSocket->peerAddress().toString();
        writeResponse(clientSocket, response);
        return;
    }
    auto emergencyData = mDbManager->emergencyData(error);
    if (error.type() != QSqlError::NoError) {
        qCritical() << "Problem loading emergency data:" << error;
        writeResponse(clientSocket, response);
        return;
    }

    ControllerInfo info;
    if (mControllers.contains(request.roomId())) {
        info = mControllers[request.roomId()];
    } else {
        info.controllerUuid = request.roomId();
        info.controllerAddress = clientSocket->peerAddress();
        info.controllerPort = 12000 + mControllers.size();
        mControllers[info.controllerUuid] = info;
    }
    response.setSuccess(true);
    response.setPort(info.controllerPort);
    response.setEmergencyData(emergencyData);
    responseData = response.toJsonString();
    writeResponse(clientSocket, response);
    qInfo() << "Registered controller" << response.roomId().toString() << "from"
            << info.controllerAddress.toString();
}

void PasoServer::handleAccessRequest(QTcpSocket *clientSocket,
                                     const QString &requestString) {
    QSqlError error;
    AccessRequest request;
    request.fromJsonString(requestString);
    AccessResponse response(request.roomId());
    response.setGranted(false);
    auto teachersOnly = !mControllers.contains(request.roomId());
    response.setReRegister(teachersOnly);
    if (teachersOnly) {
        qWarning() << "Got access query request for" << request.rfid()
                   << "from non registered controller"
                   << request.roomId().toString();
    } else {
        qInfo() << "Got access query request for" << request.rfid() << "from"
                << request.roomId().toString();
    }
    auto granted = mDbManager->checkAccess(request.roomId(), request.rfid(),
                                           teachersOnly, error);
    if (error.type() != QSqlError::NoError) {
        qCritical() << "Problem checking access:" << error;
        writeResponse(clientSocket, response);
        return;
    }
    if (!granted) {
        if (teachersOnly) {
            qWarning() << "Access denied for" << request.rfid() << "for room"
                       << request.roomId().toString()
                       << "and reregister notification sent.";
        } else {
            qInfo() << "Access denied for" << request.rfid() << "for room"
                    << request.roomId().toString();
        }
        writeResponse(clientSocket, response);
        return;
    }
    if (teachersOnly) {
        qInfo() << "Access granted for" << request.rfid() << "for room"
                << request.roomId().toString()
                << "and reregister notification sent.";
    } else {
        qInfo() << "Access granted for" << request.rfid() << "for room"
                << request.roomId().toString();
    }
    response.setGranted(true);
    writeResponse(clientSocket, response);
}

void PasoServer::checkControllers() {
    purgeLists();
    if (mControllers.isEmpty()) {
        qInfo() << "No registered controllers. Skipping check.";
        QTimer::singleShot(mControllerCheckPeriod, this,
                           &PasoServer::checkControllers);
        return;
    }
    qInfo() << "Checking controllers.";
    QSqlError error;
    auto emergencyData = mDbManager->emergencyData(error);
    if (error.type() != QSqlError::NoError) {
        qCritical() << "Problem loading emergency data. Controller check not "
                       "possible. Error was:"
                    << error;
        return;
    }
    bool allOk = true;
    for (const auto &uuid : mControllers.keys()) {
        qInfo() << "Checking controller" << uuid.toString();
        bool ok = checkController(uuid, emergencyData);
        allOk = allOk && ok;
    }
    if (allOk) {
        qInfo() << "All controllers reported that they are OK.";
    } else {
        qWarning() << "Some controllers are not OK!";
    }
    QTimer::singleShot(mControllerCheckPeriod, this,
                       &PasoServer::checkControllers);
}

bool PasoServer::checkController(const QUuid &uuid,
                                 const QStringList &emergencyData) {
    auto controller = mControllers[uuid];
    PingRequest request(uuid, emergencyData);
    QSslSocket socket;
    socket.setPeerVerifyMode(QSslSocket::VerifyNone);
    socket.connectToHostEncrypted(controller.controllerAddress.toString(),
                                  controller.controllerPort);
    if (!socket.waitForEncrypted(mTimeout)) {
        qWarning() << "The controller" << uuid.toString() << "at address"
                   << controller.controllerAddress.toString()
                   << "is either down or not listening on port"
                   << controller.controllerPort
                   << "error was:" << socket.errorString();
        return false;
    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);
    out << quint16(0);
    out << request.toJsonString();
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    socket.write(block);
    if (!socket.waitForBytesWritten(mTimeout)) {
        qWarning() << "The controller" << uuid.toString() << "at address"
                   << controller.controllerAddress.toString()
                   << "listening on port" << controller.controllerPort
                   << "failed to accept ping request. Error was:"
                   << socket.errorString();
        return false;
    }
    if (!socket.waitForReadyRead(mTimeout)) {
        qWarning() << "The controller" << uuid.toString() << "at address"
                   << controller.controllerAddress.toString()
                   << "listening on port" << controller.controllerPort
                   << "failed to reply. Error was:" << socket.errorString();
        return false;
    }
    quint16 blockSize;
    QDataStream in(&socket);
    in.setVersion(QDataStream::Qt_5_5);
    in >> blockSize;

    while (socket.bytesAvailable() < blockSize) {
        if (!socket.waitForReadyRead()) {
            qWarning() << "The controller" << uuid.toString() << "at address"
                       << controller.controllerAddress.toString()
                       << "listening on port" << controller.controllerPort
                       << "failed to reply. Error was:" << socket.errorString();
            return false;
        }
    }

    QString response;
    in >> response;
    PingResponse pingResponse;
    pingResponse.fromJsonString(response);
    if (pingResponse.roomId() != request.roomId()) {
        qWarning() << "The controller" << uuid.toString() << "at address"
                   << controller.controllerAddress.toString()
                   << "listening on port" << controller.controllerPort
                   << "replied with wrong UUID"
                   << pingResponse.roomId().toString();
        return false;
    }

    if (!pingResponse.response()) {
        qWarning() << "The controller" << uuid.toString() << "at address"
                   << controller.controllerAddress.toString()
                   << "listening on port" << controller.controllerPort
                   << "reports that it has problems. The problem is"
                   << pingResponse.fault();
        return false;
    }

    qInfo() << "The controller" << uuid.toString() << "at address"
            << controller.controllerAddress.toString() << "listening on port"
            << controller.controllerPort << "reports that everything is OK.";
    return true;
}

void PasoServer::purgeLists() {
    qInfo() << "Purging expired lists.";
    auto db = QSqlDatabase::database(mDatabaseName);
    db.exec("DELETE FROM LIST WHERE EXPIRY_DATE < CURRENT_TIMESTAMP");
    auto error = db.lastError();
    if (error.type() != QSqlError::NoError) {
        qWarning() << "Failed to purge lists. " << error;
    }
}

}
}
