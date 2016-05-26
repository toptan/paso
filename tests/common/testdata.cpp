#include "testdata.h"

#include "data.h"

#include <QDebug>
#include <QJsonDocument>
#include <QUuid>

using namespace paso::data;

void TestData::testComparingObjectWithItselfIsAlwaysTrue() {
    SystemUser user("user", "user_pass", "John", "Doe", "john.doe@internet.com",
                    SystemRole::MANAGER);
    auto room = new Room(QUuid::createUuid().toString(), "Room 42", "42");
    QVERIFY(user == user);
    QVERIFY(*room == *room);
    delete room;
}

void TestData::testRoomSerialization() {
    Room expected(QUuid::createUuid().toString(), "Room 42", "42");
    QString jsonString = expected.write();
    Room deserialized("", "", "");
    deserialized.read(jsonString);
    QCOMPARE(deserialized, expected);
}

void TestData::testSystemUserSerialization() {
    SystemUser expected("user", "user_pass", "John", "Doe",
                        "john.doe@internet.com", SystemRole::MANAGER);
    QString jsonString = expected.write();
    SystemUser deserialized("");
    deserialized.read(jsonString);
    QCOMPARE(deserialized, expected);
}

void TestData::testSystemRoleSerialization() {
    SystemUser expected("user", "user_pass", "John", "Doe",
                        "john.doe@internet.com", SystemRole::ADMINISTRATOR);
    auto jsonString = expected.write();
    SystemUser deserialized("");
    deserialized.read(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::ADMINISTRATOR);
    expected.setRole(SystemRole::ROOM_MANAGER);
    jsonString = expected.write();
    deserialized.read(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::ROOM_MANAGER);
    expected.setRole(SystemRole::MANAGER);
    jsonString = expected.write();
    deserialized.read(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::MANAGER);
    expected.setRole(SystemRole::SCHEDULER);
    jsonString = expected.write();
    deserialized.read(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::SCHEDULER);
    expected.setRole(SystemRole::SUPER_USER);
    jsonString = expected.write();
    deserialized.read(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::SUPER_USER);
}

// QTEST_MAIN(TestData)
