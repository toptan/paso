#ifndef TESTCOMMMANAGER_H
#define TESTCOMMMANAGER_H

#include "commmanager.h"
#include "testbase.h"

#include <QTcpServer>
#include <QTest>

#include <memory>

using namespace paso::comm;

class DemoServer;

class TestCommManager : public TestBase {
    Q_OBJECT

public:
    TestCommManager();

public slots:
    void loginSuccessfull(const LoginResponse &resonse);
    void loginFailed();

signals:
    void quitLocalEventLoop();

protected slots:

    virtual void initTestCase() override;
    virtual void init() override;

private slots:
    void testSuccessfullLogin();
    void testUnsuccessfullLogin();

private:
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
