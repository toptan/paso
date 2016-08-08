#ifndef TESTLOGINDIALOG_H
#define TESTLOGINDIALOG_H

#include "testbase.h"

#include <QTcpServer>
#include <QTest>
#include <QTest>

class MockServer;

class TestLoginDialog : public TestBase {
    Q_OBJECT
public:
    TestLoginDialog();

private slots:
    void testEntryValidation();
    void testLogin();
};

class MockServer : public QObject {
    Q_OBJECT

public:
    explicit MockServer(QObject *parent = nullptr);

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
#endif // TESTLOGINDIALOG_H
