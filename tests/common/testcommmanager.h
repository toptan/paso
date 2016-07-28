#ifndef TESTCOMMMANAGER_H
#define TESTCOMMMANAGER_H

#include <QTest>
#include <QTcpServer>
#include "commmanager.h"

#include <memory>

using namespace paso::comm;

class DemoServer;

class TestCommManager : public QObject {
    Q_OBJECT

public:
    TestCommManager();

public slots:
    void loginSuccessfull(const LoginResponse &resonse);
    void loginFailed();

signals:
    void quitLocalEventLoop();

private slots:

    void initTestCase();
    void init();
    void cleanup();

    void testSuccessfullLogin();
    void testUnsuccessfullLogin();

private:
    const QString dbName;
    std::shared_ptr<CommManager> manager;
    DemoServer *demoServer;
    bool loginSuccess;
    bool loginFailure;
};

class DemoServer : public QObject {
    Q_OBJECT

public:
    explicit DemoServer(QObject *parent = nullptr);

private slots:
    void handleRequest();

private:
    QTcpServer *tcpServer;
    const unsigned short mPort;
    const int mTimeout;
    const QString mDbName;
    void handleLoginRequest(QTcpSocket *clientSocket,
                            const QString &requestString);
};

#endif // TESTCOMMMANAGER_H
