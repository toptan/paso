#include "testcommdata.h"

#include "commdata.h"

#include <QDebug>
#include <QJsonDocument>

using namespace paso::comm;
using namespace paso::data;
using namespace paso::data::entity;

TestCommData::TestCommData() : TestBase() {}

void TestCommData::testBaseSerialization() {
    Base expected(QUuid::createUuid(), Operation::UNKNOWN_OPERATION);
    Base deserialized("{00000000-0000-0000-0000-000000000000}",
                      Operation::UNKNOWN_OPERATION);
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
