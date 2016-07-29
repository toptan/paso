#include "testcommmanager.h"

#include "commmanager.h"
#include "pasodb.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QTcpSocket>
#include <QtConcurrent>
#include <functional>

using namespace paso::db;
using namespace paso::comm;
using namespace paso::data;

DemoServer::DemoServer(QObject *parent)
    : QObject(parent), mPort(16789), mTimeout(5000), mDbName(DEFAULT_DB_NAME) {
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this,
            &DemoServer::handleRequest);
    tcpServer->listen(QHostAddress::LocalHost, mPort);
}

void DemoServer::handleRequest() {
    auto clientSocket = tcpServer->nextPendingConnection();
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

void DemoServer::handleLoginRequest(QTcpSocket *clientSocket,
                                    const QString &requestString) {
    QString responseData;
    LoginRequest loginRequest;
    loginRequest.fromJsonString(requestString);
    DBManager manager(mDbName);
    QSqlError error;
    auto sysUser = manager.getSystemUser(loginRequest.username(), error);
    if (sysUser && sysUser->password() == loginRequest.password()) {
        LoginResponse response(*sysUser, "db_name", "db_server", "db_username",
                               "db_password", 5432);
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

TestCommManager::TestCommManager() : TestBase() {}

void TestCommManager::initTestCase() {
    TestBase::initTestCase();
    qRegisterMetaType<LoginResponse>("LoginResponse");
    manager = std::make_shared<CommManager>("localhost", 16789);
    connect(manager.get(), &CommManager::loginSuccessfull, this,
            &TestCommManager::loginSuccessfull);
    connect(manager.get(), &CommManager::loginFailed, this,
            &TestCommManager::loginFailed);
    demoServer = new DemoServer(this);
}

void TestCommManager::init() {
    TestBase::init();
    loginSuccess = false;
    loginFailure = false;
}

void TestCommManager::loginSuccessfull(const LoginResponse &resonse) {
    loginSuccess = true;
    loginFailure = false;
    emit quitLocalEventLoop();
}

void TestCommManager::loginFailed() {
    loginSuccess = false;
    loginFailure = true;
    emit quitLocalEventLoop();
}

void TestCommManager::testSuccessfullLogin() {
    QEventLoop loop;
    connect(this, &TestCommManager::quitLocalEventLoop, &loop,
            &QEventLoop::quit);
    manager->login("scheduler", "scheduler_password");
    loop.exec();
    QVERIFY(loginSuccess && !loginFailure);
}

void TestCommManager::testUnsuccessfullLogin() {
    QEventLoop loop;
    connect(this, &TestCommManager::quitLocalEventLoop, &loop,
            &QEventLoop::quit);
    manager->login("Foo", "");
    loop.exec();
    QVERIFY(!loginSuccess && loginFailure);
}
