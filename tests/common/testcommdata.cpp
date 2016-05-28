#include "testcommdata.h"

#include "commdata.h"

#include <QDebug>
#include <QJsonDocument>

using namespace paso::comm;
using namespace paso::data;

void TestCommData::testLoginResponseCreation() {
    SystemUser sysUser("user", "user_pass", "John", "Doe",
                       "john.doe@internet.com", SystemRole::ADMINISTRATOR);
    LoginResponse *response = new LoginResponse(sysUser, "A", "B", "C", "D", 1);
    QCOMPARE(response->systemUser(), sysUser);
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
    LoginResponse expected(sysUser, "A", "B", "C", "D", 1);
    LoginResponse deserialized;
    QString jsonString = expected.toJsonString();
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized.systemUser(), expected.systemUser());
    QVERIFY(deserialized.dbName() == expected.dbName());
    QVERIFY(deserialized.dbServer() == expected.dbServer());
    QVERIFY(deserialized.dbUsername() == expected.dbUsername());
    QVERIFY(deserialized.dbPassword() == expected.dbPassword());
    QVERIFY(deserialized.dbPort() == expected.dbPort());
}

QTEST_MAIN(TestCommData)
