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
};

#endif // TESTLOGINDIALOG_H
