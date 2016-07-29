#include "testdata.h"

#include "course.h"
#include "data.h"
#include "list.h"
#include "room.h"
#include "student.h"
#include "systemuser.h"

#include <QDebug>
#include <QJsonDocument>
#include <QUuid>
#include <memory>

using namespace std;
using namespace paso::data;
using namespace paso::data::entity;

TestData::TestData() : TestBase() {}

void TestData::testComparingObjectWithItselfIsAlwaysTrue() {
    SystemUser user("user", "user_pass", "John", "Doe", "john.doe@internet.com",
                    SystemRole::MANAGER);
    auto room = new Room(QUuid::createUuid().toString(), "Room 42", "42");
    shared_ptr<Course> course =
        make_shared<Course>("IR3SP", "Sistemsko programiranje");
    Student student("Toplica", "Tanasković", "toptan@foo.com", "164/96", 5, 123,
                    "RRFFIIDD");
    List list("List name", false, 4);
    QVERIFY(user == user);
    QVERIFY(*room == *room);
    QVERIFY(*course == *course);
    QVERIFY(student == student);
    QVERIFY(list == list);
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

void TestData::testListSerialization() {
    List expected("Demo lista", true, 4);
    QString jsonString = expected.toJsonString();
    List deserialized;
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized, expected);
}

void TestData::testConversionToVariantMap() {
    SystemUser systemUser("user", "user_pass", "John", "Doe",
                          "john.doe@internet.com", SystemRole::MANAGER);
    Student student("Toplica", "Tanasković", "toptan@foo.com", "164/96", 5, 123,
                    "RRFFIIDD");
    Course course("IR3SP", "Sistemsko programiranje", 8);
    Room room(QUuid::createUuid().toString(), "Room 42", "42");
    List list("Demo lista", false, 4);

    QStringList systemUserKeys{"ID",         "USERNAME",  "PASSWORD",
                               "FIRST_NAME", "LAST_NAME", "EMAIL",
                               "ROLE"};

    QStringList studentKeys{"ID",    "FIRST_NAME",   "LAST_NAME",
                            "EMAIL", "INDEX_NUMBER", "YEAR_OF_STUDY",
                            "RFID"};

    QStringList courseKeys{"ID", "CODE", "NAME"};

    QStringList roomKeys{"ID", "ROOM_UUID", "NAME", "ROOM_NUMBER"};

    QStringList listKeys{"ID", "NAME", "SYSTEM", "PERMANENT", "EXPIRY_DATE"};

    QCOMPARE(systemUser.toVariantMap().keys().size(), systemUserKeys.size());
    QCOMPARE(student.toVariantMap().keys().size(), studentKeys.size());
    QCOMPARE(course.toVariantMap().keys().size(), courseKeys.size());
    QCOMPARE(room.toVariantMap().keys().size(), roomKeys.size());
    QCOMPARE(list.toVariantMap().keys().size(), listKeys.size());

    for (const auto &key : systemUser.toVariantMap().keys()) {
        systemUserKeys.removeOne(key);
    }
    QVERIFY(systemUserKeys.empty());
    for (const auto &key : student.toVariantMap().keys()) {
        studentKeys.removeOne(key);
    }
    QVERIFY(studentKeys.empty());
    for (const auto &key : course.toVariantMap().keys()) {
        courseKeys.removeOne(key);
    }
    QVERIFY(courseKeys.empty());
    for (const auto &key : room.toVariantMap().keys()) {
        roomKeys.removeOne(key);
    }
    QVERIFY(roomKeys.empty());
    for (const auto &key : list.toVariantMap().keys()) {
        listKeys.removeOne(key);
    }
    QVERIFY(listKeys.empty());
}

void TestData::testPropertyValues() {
    SystemUser systemUser("user", "user_pass", "John", "Doe",
                          "john.doe@internet.com", SystemRole::MANAGER, 6);
    QCOMPARE(systemUser.value("FOO"), QVariant());
    QCOMPARE(systemUser.value("ID"),
             QVariant(static_cast<quint64>(systemUser.id())));
    QCOMPARE(systemUser.value("USERNAME"), QVariant(systemUser.username()));
    QCOMPARE(systemUser.value("PASSWORD"), QVariant(systemUser.password()));
    QCOMPARE(systemUser.value("FIRST_NAME"), QVariant(systemUser.firstName()));
    QCOMPARE(systemUser.value("LAST_NAME"), QVariant(systemUser.lastName()));
    QCOMPARE(systemUser.value("EMAIL"), QVariant(systemUser.email()));
    QCOMPARE(systemUser.value("ROLE"),
             QVariant(roleToString(systemUser.role())));

    Student student("Toplica", "Tanasković", "toptan@foo.com", "164/96", 5, 123,
                    "RRFFIIDD");
    QCOMPARE(student.value("FOO"), QVariant());
    QCOMPARE(student.value("ID"), QVariant(static_cast<quint64>(student.id())));
    QCOMPARE(student.value("FIRST_NAME"), QVariant(student.firstName()));
    QCOMPARE(student.value("LAST_NAME"), QVariant(student.lastName()));
    QCOMPARE(student.value("EMAIL"), QVariant(student.email()));
    QCOMPARE(student.value("INDEX_NUMBER"), QVariant(student.indexNumber()));
    QCOMPARE(student.value("YEAR_OF_STUDY"), QVariant(student.yearOfStudy()));
    QCOMPARE(student.value("RFID"), QVariant(student.rfid()));

    Course course("IR3SP", "Sistemsko programiranje", 8);
    QCOMPARE(course.value("FOO"), QVariant());
    QCOMPARE(course.value("ID"), QVariant(static_cast<quint64>(course.id())));
    QCOMPARE(course.value("CODE"), QVariant(course.code()));
    QCOMPARE(course.value("NAME"), QVariant(course.name()));

    QString uuid = QUuid::createUuid().toString();
    Room room(uuid, "Room 42", "42");
    QCOMPARE(room.value("FOO"), QVariant());
    QCOMPARE(room.value("ID"), QVariant(static_cast<quint64>(room.id())));
    QCOMPARE(room.value("ROOM_UUID"), QVariant(uuid));
    QCOMPARE(room.value("ROOM_UUID"), QVariant(room.roomUUID()));
    QCOMPARE(room.value("NAME"), QVariant(room.name()));
    QCOMPARE(room.value("ROOM_NUMBER"), QVariant(room.number()));

    List list("Demo lista", true, 4);
    QCOMPARE(list.value("FOO"), QVariant());
    QCOMPARE(list.value("ID"), QVariant(static_cast<quint64>(list.id())));
    QCOMPARE(list.value("NAME"), QVariant(list.name()));
    QCOMPARE(list.value("SYSTEM"), QVariant(list.system()));
    QCOMPARE(list.value("PERMANENT"), QVariant(list.permanent()));
    QCOMPARE(list.value("EXPIRY_DATE"), QVariant::fromValue(QDate()));
}
