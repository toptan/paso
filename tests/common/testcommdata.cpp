#include "testcommdata.h"

#include "commaccess.h"
#include "commdata.h"
#include "commping.h"
#include "commregister.h"
#include "sslserver.h"

#include <QDebug>
#include <QEventLoop>
#include <QJsonDocument>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslSocket>
#include <QTextStream>
#include <QTimer>
#include <QtConcurrent>

#include <memory>

using namespace paso::comm;
using namespace paso::data;
using namespace paso::data::entity;

TestCommData::TestCommData() : TestBase() {}

void TestCommData::testBaseSerialization() {
    Base expected(QUuid::createUuid(), Operation::UNKNOWN_OPERATION);
    Base deserialized("{00000000-0000-0000-0000-000000000000}");
    auto jsonString = expected.toJsonString();
    deserialized.fromJsonString(jsonString);
    QVERIFY(deserialized.roomId() == expected.roomId());
    QVERIFY(deserialized.operation() == expected.operation());
}

void TestCommData::testLoginRequestCreation() {
    auto request = new LoginRequest("foo", "bar");
    QVERIFY(request->roomId().isNull());
    QVERIFY(request->operation() == Operation::LOGIN);
    QVERIFY(request->username() == "foo");
    QVERIFY(request->password() == "bar");
    delete request;
}

void TestCommData::testLoginRequestSerialization() {
    LoginRequest expected("john.doe", "jdpass");
    LoginRequest deserialized;
    auto jsonString = expected.toJsonString();
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized.username(), expected.username());
    QCOMPARE(deserialized.password(), expected.password());
}

void TestCommData::testLoginResponseCreation() {
    SystemUser sysUser("user", "user_pass", "John", "Doe",
                       "john.doe@internet.com", SystemRole::ADMINISTRATOR);
    LoginResponse *response =
        new LoginResponse(sysUser, "QSQLITE", "A", "B", "C", "D", 1);
    QVERIFY(response->operation() == Operation::LOGIN);
    QCOMPARE(response->systemUser(), sysUser);
    QVERIFY(response->dbType() == "QSQLITE");
    QVERIFY(response->dbName() == "A");
    QVERIFY(response->dbServer() == "B");
    QVERIFY(response->dbUsername() == "C");
    QVERIFY(response->dbPassword() == "D");
    QVERIFY(response->dbPort() == 1);
    delete response;
}

void TestCommData::testLoginResponseSerialization() {
    SystemUser sysUser("user", "user_pass", "John", "Doe",
                       "john.doe@internet.com", SystemRole::ADMINISTRATOR);
    LoginResponse expected(sysUser, "QSQLITE", "A", "B", "C", "D", 1);
    LoginResponse deserialized;
    QString jsonString = expected.toJsonString();
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized.systemUser(), expected.systemUser());
    QVERIFY(deserialized.dbType() == expected.dbType());
    QVERIFY(deserialized.dbName() == expected.dbName());
    QVERIFY(deserialized.dbServer() == expected.dbServer());
    QVERIFY(deserialized.dbUsername() == expected.dbUsername());
    QVERIFY(deserialized.dbPassword() == expected.dbPassword());
    QVERIFY(deserialized.dbPort() == expected.dbPort());
}

void TestCommData::testRegisterRequestSerialization() {
    RegisterRequest expected(QUuid::createUuid());
    RegisterRequest deserialized;
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.operation(), expected.operation());
    QCOMPARE(deserialized.roomId(), expected.roomId());
}

void TestCommData::testRegisterResponseSerialization() {
    RegisterResponse expected(QUuid::createUuid());
    expected.setSuccess(true);
    expected.setPort(1234);
    expected.setEmergencyData({"A", "B"});
    RegisterResponse deserialized;
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.operation(), expected.operation());
    QCOMPARE(deserialized.roomId(), expected.roomId());
    QCOMPARE(deserialized.success(), expected.success());
    QCOMPARE(deserialized.port(), expected.port());
    QCOMPARE(deserialized.emergencyData(), expected.emergencyData());
}

void TestCommData::testAccessRequestSerialization() {
    AccessRequest expected(QUuid::createUuid());
    expected.setRfid("RRFFIIDD");
    AccessRequest deserialized;
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.operation(), expected.operation());
    QCOMPARE(deserialized.roomId(), expected.roomId());
    QCOMPARE(deserialized.rfid(), expected.rfid());
}

void TestCommData::testAccessResponseSerialization() {
    AccessResponse expected(QUuid::createUuid());
    expected.setGranted(true);
    expected.setReRegister(true);
    AccessResponse deserialized;
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.operation(), expected.operation());
    QCOMPARE(deserialized.roomId(), expected.roomId());
    QCOMPARE(deserialized.granted(), expected.granted());
    QCOMPARE(deserialized.reRegister(), expected.reRegister());
}

void TestCommData::testPingRequestSerialization() {
    PingRequest expected(QUuid::createUuid());
    expected.setEmergencyData({"A", "B", "C"});
    PingRequest deserialized;
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.operation(), expected.operation());
    QCOMPARE(deserialized.roomId(), expected.roomId());
    QCOMPARE(deserialized.emergencyData(), expected.emergencyData());
}

void TestCommData::testPingResponseSerialization() {
    PingResponse expected(QUuid::createUuid());
    expected.setResponse(false);
    expected.setFault("An error");
    PingResponse deserialized;
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.operation(), expected.operation());
    QCOMPARE(deserialized.roomId(), expected.roomId());
    QCOMPARE(deserialized.response(), expected.response());
    QCOMPARE(deserialized.fault(), expected.fault());
}

void TestCommData::testSslServer() {
    QFile keyFile(SERVER_KEY_PATH);
    QFile certFile(SERVER_CERT_PATH);
    QVERIFY(keyFile.exists());
    QVERIFY(certFile.exists());
    QVERIFY(keyFile.open(QIODevice::ReadOnly));
    QVERIFY(certFile.open(QIODevice::ReadOnly));

    auto key = std::make_shared<QSslKey>(&keyFile, QSsl::Rsa, QSsl::Pem,
                                         QSsl::PrivateKey);
    auto cert = std::make_shared<QSslCertificate>(&certFile);

    bool commSuccessful = false;
    QEventLoop localLoop;
    auto sslServer = new SslServer(cert, key, this);
    connect(sslServer, &QTcpServer::newConnection,
            [&commSuccessful, &localLoop, &sslServer]() {
                auto serverSocket = sslServer->nextPendingConnection();
                connect(serverSocket, &QTcpSocket::disconnected, serverSocket,
                        &QObject::deleteLater);
                connect(serverSocket, &QTcpSocket::disconnected, &localLoop,
                        &QEventLoop::quit);

                if (!serverSocket->waitForReadyRead()) {
                    serverSocket->disconnectFromHost();
                    return;
                }

                QByteArray data;
                quint16 blockSize = 0;
                QDataStream in(serverSocket);
                in.setVersion(QDataStream::Qt_5_5);
                in >> blockSize;

                while (serverSocket->bytesAvailable() < blockSize) {
                    if (!serverSocket->waitForReadyRead()) {
                        serverSocket->disconnectFromHost();
                        return;
                    }
                }
                in >> data;
                QString message(data);
                if (message == "Hello!") {
                    commSuccessful = true;
                } else {
                    commSuccessful = false;
                }
                serverSocket->disconnectFromHost();
            });

    QVERIFY(sslServer->listen(QHostAddress::LocalHost, 9595));
    QtConcurrent::run([&localLoop]() {
        QSslSocket socket;
        connect(&socket, &QTcpSocket::disconnected, &localLoop,
                &QEventLoop::quit);
        socket.setPeerVerifyMode(QSslSocket::VerifyNone);
        socket.connectToHostEncrypted("localhost", 9595);
        if (!socket.waitForEncrypted()) {
            qWarning() << socket.errorString();
            socket.disconnectFromHost();
            return;
        }

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_5);
        out << (quint16)0;
        out << "Hello!";
        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        socket.write(block);
        socket.waitForBytesWritten();
        socket.waitForReadyRead();
    });
    localLoop.exec();
    QVERIFY(commSuccessful);
}
