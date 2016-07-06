#include "testdata.h"

#include "data.h"
#include "systemuser.h"
#include "room.h"
#include "course.h"
#include "student.h"

#include <memory>
#include <QDebug>
#include <QJsonDocument>
#include <QUuid>

using namespace paso::data;
using namespace paso::data::entity;

void TestData::testComparingObjectWithItselfIsAlwaysTrue() {
    SystemUser user("user", "user_pass", "John", "Doe", "john.doe@internet.com",
                    SystemRole::MANAGER);
    auto room = new Room(QUuid::createUuid().toString(), "Room 42", "42");
    std::shared_ptr<Course> course =
        std::make_shared<Course>("IR3SP", "Sistemsko programiranje");
    Student student("Toplica", "Tanasković", "toptan@foo.com", "164/96", 5, 123,
                    "RRFFIIDD");
    QVERIFY(user == user);
    QVERIFY(*room == *room);
    QVERIFY(*course == *course);
    QVERIFY(student == student);
    delete room;
}

void TestData::testComparingStudentWithPersonOrProfessorIsAlwaysFalse() {
    Student student("Toplica", "Tanasković", "toptan@foo.com", "164/96", 5, 123,
                    "RRFFIIDD");
    Person person("Toplica", "Tanaskovic", "toptan@foo.com", 123, "RRFFIIDD");
    QVERIFY(!(student == person));
}

void TestData::testRoomSerialization() {
    Room expected(QUuid::createUuid().toString(), "Room 42", "42");
    QString jsonString = expected.toJsonString();
    Room deserialized("", "", "");
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized, expected);
}

void TestData::testSystemUserSerialization() {
    SystemUser expected("user", "user_pass", "John", "Doe",
                        "john.doe@internet.com", SystemRole::MANAGER);
    QString jsonString = expected.toJsonString();
    SystemUser *deserialized = new SystemUser("");
    deserialized->fromJsonString(jsonString);
    QCOMPARE(*deserialized, expected);
    delete deserialized;
}

void TestData::testSystemRoleSerialization() {
    SystemUser expected("user", "user_pass", "John", "Doe",
                        "john.doe@internet.com", SystemRole::ADMINISTRATOR);
    auto jsonString = expected.toJsonString();
    SystemUser deserialized("");
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::ADMINISTRATOR);
    expected.setRole(SystemRole::ROOM_MANAGER);
    jsonString = expected.toJsonString();
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::ROOM_MANAGER);
    expected.setRole(SystemRole::MANAGER);
    jsonString = expected.toJsonString();
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::MANAGER);
    expected.setRole(SystemRole::SCHEDULER);
    jsonString = expected.toJsonString();
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::SCHEDULER);
    expected.setRole(SystemRole::SUPER_USER);
    jsonString = expected.toJsonString();
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::SUPER_USER);
}

void TestData::testCourseSerialization() {
    Course expected("IR3SP", "Sistemsko programiranje");
    QString jsonString = expected.toJsonString();
    auto deserialized = new Course("", "");
    deserialized->fromJsonString(jsonString);
    QCOMPARE(*deserialized, expected);
    delete deserialized;
}

void TestData::testStudentSerialization() {
    Student expected("Toplica", "Tanaskovic", "toptan@foo.com", "164/96", 5,
                     123, "RRFFIIDD");
    QString jsonString = expected.toJsonString();
    Student deserialized((QVariantMap()));
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized, expected);
}

QTEST_MAIN(TestData)
