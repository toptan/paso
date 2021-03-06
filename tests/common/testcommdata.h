#ifndef TESTCOMMDATA_H
#define TESTCOMMDATA_H

#include "testbase.h"
#include <QTest>

class TestCommData : public TestBase {
    Q_OBJECT

public:
    TestCommData();

private slots:
    void testBaseSerialization();
    void testLoginRequestCreation();
    void testLoginRequestSerialization();
    void testLoginResponseCreation();
    void testLoginResponseSerialization();
    void testRegisterRequestSerialization();
    void testRegisterResponseSerialization();
    void testAccessRequestSerialization();
    void testAccessResponseSerialization();
    void testPingRequestSerialization();
    void testPingResponseSerialization();
    void testSslServer();
};

#endif // TESTCOMMDATA_H
