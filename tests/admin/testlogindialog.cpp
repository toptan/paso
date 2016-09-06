#include "testlogindialog.h"

#include "commdata.h"
#include "logindialog.h"
#include "systemuser.h"

#include <QDataStream>
#include <QLabel>
#include <QLineEdit>
#include <QTcpSocket>
#include <QtConcurrent>

using namespace paso::admin;
using namespace paso::comm;
using namespace paso::data;
using namespace paso::data::entity;

MockServer::MockServer(QObject *parent)
    : QObject(parent), mPort(6789), mTimeout(5000), mDbName() {
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this,
            &MockServer::handleRequest);
    tcpServer->listen(QHostAddress::LocalHost, mPort);
}

void MockServer::handleRequest() {
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

void MockServer::handleLoginRequest(QTcpSocket *clientSocket,
                                    const QString &requestString) {
    QString responseData;
    LoginRequest loginRequest;
    loginRequest.fromJsonString(requestString);
    if (loginRequest.username() == "allowed_user") {
        SystemUser user("root", "", "", "", "", SystemRole::SUPER_USER);
        LoginResponse response(user, "QSQLITE", "db_name", "db_server",
                               "db_username", "db_password", 5432);
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

TestLoginDialog::TestLoginDialog() : TestBase() {}

void TestLoginDialog::testEntryValidation() {
    LoginDialog dialog;
    dialog.show();
    QTest::qWaitForWindowExposed(&dialog);

    auto usernameEdit = dialog.findChild<QLineEdit *>("usernameLineEdit");
    auto passwordEdit = dialog.findChild<QLineEdit *>("passwordLineEdit");
    auto serverEdit = dialog.findChild<QLineEdit *>("serverLineEdit");
    auto messageLabel = dialog.findChild<QLabel *>("messageLabel");

    usernameEdit->clear();
    passwordEdit->clear();
    serverEdit->clear();
    dialog.accept();
    QApplication::processEvents();
    QCOMPARE(messageLabel->text(), QString("You need to provide username."));
    QVERIFY(usernameEdit->hasFocus());

    usernameEdit->setText("username");
    dialog.accept();
    QApplication::processEvents();
    QCOMPARE(messageLabel->text(), QString("You need to provide password."));
    QVERIFY(passwordEdit->hasFocus());

    passwordEdit->setText("password");
    dialog.accept();
    QApplication::processEvents();
    QCOMPARE(messageLabel->text(), QString("You need to provide server name."));
    QVERIFY(serverEdit->hasFocus());
}

void TestLoginDialog::testLogin() {
    qRegisterMetaType<LoginResponse>("LoginResponse");
    QEventLoop loop;
    LoginDialog dialog;
    connect(&dialog, &LoginDialog::loginAttemptFailed, &loop,
            &QEventLoop::quit);
    connect(&dialog, &LoginDialog::loginFinished, &loop, &QEventLoop::quit);
    dialog.show();
    QTest::qWaitForWindowExposed(&dialog);

    auto usernameEdit = dialog.findChild<QLineEdit *>("usernameLineEdit");
    auto passwordEdit = dialog.findChild<QLineEdit *>("passwordLineEdit");
    auto serverEdit = dialog.findChild<QLineEdit *>("serverLineEdit");
    auto messageLabel = dialog.findChild<QLabel *>("messageLabel");

    usernameEdit->setText("unallowed_user");
    passwordEdit->setText("password");
    serverEdit->setText("localhost");
    dialog.accept();
    loop.exec();
    QVERIFY(!messageLabel->text().isEmpty());
    MockServer mockServer;
    dialog.accept();
    loop.exec();
    QCOMPARE(messageLabel->text(), QString("Login failed."));
    usernameEdit->setText("allowed_user");
    dialog.accept();
    loop.exec();
    QVERIFY(!dialog.isVisible());
}
