#include "testpasodb.h"

#include "data.h"
#include "pasodb.h"

#include <QDebug>
#include <QSqlError>
#include <QUuid>
#include <QDataStream>

using namespace paso::db;
using namespace paso::data;
using namespace paso::data::entity;

void TestPasoDB::initTestCase() {
    dbName = "paso";
    roomUUIDs << QUuid("{d23a502b-a567-4929-ba99-9f93f36bf4e3}").toString();
    roomUUIDs << QUuid("{7003528d-4c44-4f91-91b4-b82cb5afb009}").toString();
    usernames << "admin"
              << "room_manager"
              << "manager"
              << "scheduler"
              << "root";
    auto db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName(":memory:");
    db.open();
}

void TestPasoDB::init() {
    auto db = QSqlDatabase::database(dbName);
    QFile in_memory_sql("in_memory.sql");
    in_memory_sql.open(QIODevice::ReadOnly);
    QTextStream in(&in_memory_sql);
    QString sqlString = in.readAll();
    sqlString.replace("\n", "");
    QStringList commands = sqlString.split(";");
    for (const auto &command : commands) {
        db.exec(command);
    }
    in_memory_sql.close();
}

void TestPasoDB::cleanup() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DROP TABLE SYSTEM_USER");
    db.exec("DROP TABLE COURSE");
    db.exec("DROP TABLE ROOM");
}

void TestPasoDB::testOperationsShouldFailOnDatabaseErrors() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("ALTER TABLE SYSTEM_USER RENAME TO SYSTEM_USERS");
    DBManager manager(dbName);
    QSqlError error;
    SystemUser user("toptan");
    user.setPassword("toptan_password");
    user.setFirstName("Toplica");
    user.setLastName("Tanasković");
    user.setEmail("toptan@server.com");
    user.setRole(SystemRole::SCHEDULER);
    QVERIFY(!manager.saveSystemUser(user, error));
    db.close();
    auto roomUUID = QUuid::createUuid();
    Room room(roomUUID.toString(), "Demo Room 1", "3");
    QVERIFY(!manager.saveRoom(room, error));
}

void TestPasoDB::testGetAllSystemUsers() {
    DBManager manager(dbName);
    QSqlError error;
    QStringList users = usernames;
    auto systemUsers = manager.getAllSystemUsers(error);
    QVERIFY(error.type() == QSqlError::NoError);
    QVERIFY(!systemUsers->empty());
    QVERIFY(systemUsers->size() == 5);
    for (const auto &user : *systemUsers) {
        users.removeOne(user.username());
    }
    QVERIFY2(users.empty(), "Not all users were returned!");
}

void TestPasoDB::testGetSystemUser() {
    DBManager manager(dbName);
    QSqlError error;
    auto systemUser = manager.getSystemUser("root", error);
    QVERIFY(error.type() == QSqlError::NoError);
    QVERIFY((bool)systemUser);
    QVERIFY(systemUser->username() == "root");
    QVERIFY(systemUser->password() == "root_password");
    QVERIFY(systemUser->firstName() == "Root");
    QVERIFY(systemUser->lastName() == "Rootovic");
    QVERIFY(systemUser->email() == "root@paso.system");
    QVERIFY(systemUser->role() == SystemRole::SUPER_USER);

    auto nonExistingUser = manager.getSystemUser("qrgerbeb", error);
    QVERIFY(error.type() == QSqlError::NoError);
    QVERIFY(!(bool)nonExistingUser);
}

void TestPasoDB::testSaveSystemUser() {
    DBManager manager(dbName);
    QSqlError error;
    SystemUser user("toptan");
    user.setPassword("toptan_password");
    user.setFirstName("Toplica");
    user.setLastName("Tanasković");
    user.setEmail("toptan@server.com");
    user.setRole(SystemRole::SCHEDULER);
    QVERIFY(manager.saveSystemUser(user, error));
    auto temp = manager.getSystemUser("toptan", error);
    QVERIFY(user == *temp);
    user.setEmail("toptan@paso.system");
    user.setRole(SystemRole::ADMINISTRATOR);
    QVERIFY(manager.saveSystemUser(user, error));
    temp = manager.getSystemUser("toptan", error);
    QVERIFY(user == *temp);
    temp = manager.getSystemUser("toptan", error);
    temp->setRole(SystemRole::ROOM_MANAGER);
    QVERIFY(manager.saveSystemUser(*temp, error));
    auto temp2 = manager.getSystemUser("toptan", error);
    QVERIFY(*temp2 == *temp);
    temp2->setRole(SystemRole::MANAGER);
    QVERIFY(manager.saveSystemUser(*temp2, error));
    temp = manager.getSystemUser("toptan", error);
    QVERIFY(*temp2 == *temp);
}

void TestPasoDB::testDeleteSystemUser() {
    DBManager manager(dbName);
    QSqlError error;
    QVERIFY(!manager.deleteSystemUser("root", error));
    QVERIFY(error.type() == QSqlError::NoError);
    QVERIFY(manager.getAllSystemUsers(error)->size() == 5);
    QVERIFY(manager.deleteSystemUser("room_manager", error));
    QVERIFY(error.type() == QSqlError::NoError);
    QVERIFY(manager.getAllSystemUsers(error)->size() == 4);
    auto user = manager.getSystemUser("room_manager", error);
    QVERIFY(!user);
}

void TestPasoDB::testGetAllRooms() {
    DBManager manager(dbName);
    QSqlError error;
    auto rooms = manager.getAllRooms(error);
    QVERIFY(error.type() == QSqlError::NoError);
    QVERIFY(!rooms->empty());
    QVERIFY(rooms->size() == 2);
    QStringList fetchedUUIDs;
    for (const auto &room : *rooms) {
        fetchedUUIDs << room.roomUUID();
    }
    QVERIFY(fetchedUUIDs == roomUUIDs);
}

void TestPasoDB::testSaveRoom() {
    DBManager manager(dbName);
    QSqlError error;
    auto roomUUID = QUuid::createUuid();
    Room room(roomUUID.toString(), "Demo Room 1", "3");
    QVERIFY(manager.saveRoom(room, error));
    auto loadedRoom = manager.getRoom(roomUUID, error);
    QVERIFY(room == *loadedRoom);
    loadedRoom->setName("Demo Lab 1");
    loadedRoom->setNumber("DL1");
    QVERIFY(manager.saveRoom(*loadedRoom, error));
    auto updatedRoom = manager.getRoom(roomUUID, error);
    QVERIFY(*updatedRoom == *loadedRoom);
}

void TestPasoDB::testGetRoom() {
    DBManager manager(dbName);
    QSqlError error;
    QUuid roomUUID(roomUUIDs[1]);
    auto room = manager.getRoom(roomUUID, error);
    QVERIFY(error.type() == QSqlError::NoError);
    QVERIFY((bool)room);
    QVERIFY(room->roomUUID() == roomUUIDs[1]);
    QVERIFY(room->name() == "Paviljon 26");
    QVERIFY(room->number() == "P26");
}

void TestPasoDB::testDeleteRoom() {
    DBManager manager(dbName);
    QSqlError error;
    QUuid roomUUID(roomUUIDs[0]);
    QVERIFY(manager.deleteRoom(roomUUID, error));
    QVERIFY(manager.getAllRooms(error)->size() == 1);
    auto room = manager.getRoom(roomUUID, error);
    QVERIFY(!room);
}

void TestPasoDB::testSaveCourse() {
    DBManager manager(dbName);
    QSqlError error;
    Course course;
    course.setCode("IR3BP1");
    course.setName("Baze podataka");
    QVERIFY(manager.saveCourse(course, error));
    auto loadedCourse = manager.getCourse("IR3BP1", error);
    QVERIFY(course == *loadedCourse);
    loadedCourse->setName("Baze podataka 1");
    QVERIFY(manager.saveCourse(*loadedCourse, error));
    auto updatedCourse = manager.getCourse("IR3BP1", error);
    QVERIFY(*updatedCourse == *loadedCourse);
}

void TestPasoDB::testGetCourse() {
    DBManager manager(dbName);
    QSqlError error;
    auto course = manager.getCourse("IR3SP", error);
    QVERIFY(error.type() == QSqlError::NoError);
    QVERIFY((bool)course);
    QVERIFY(course->code() == "IR3SP");
    QVERIFY(course->name() == "Sistemsko programiranje");
}

void TestPasoDB::testDeleteCourse() {
    DBManager manager(dbName);
    QSqlError error;
    // Make sure that course exists.
    auto course = manager.getCourse("IR3SP", error);
    QVERIFY((bool)course);
    QVERIFY(manager.deleteCourse("IR3SP", error));
    // The course should not exist anymore.
    course = manager.getCourse("IR3SP", error);
    QVERIFY(!course);
}

void TestPasoDB::testUsernameUnique() {
    DBManager manager(dbName);
    QSqlError error;
    QVERIFY(manager.usernameUnique("ChiChaGlisha", error));
    QVERIFY(!manager.usernameUnique("root", error));
}

void TestPasoDB::testRoomUuidUnique() {
    DBManager manager(dbName);
    QSqlError error;
    // This uuid is from in_memory.sql script so it should exist.
    QVERIFY(!manager.roomUuidUnique("{d23a502b-a567-4929-ba99-9f93f36bf4e3}",
                                    error));
    QVERIFY(manager.roomUuidUnique(QUuid::createUuid().toString(), error));
}

void TestPasoDB::testRoomNumberUnique() {
    DBManager manager(dbName);
    QSqlError error;
    // This room number is from in_memory.sql script so it should exist.
    QVERIFY(!manager.roomNumberUnique("P26", error));
    QVERIFY(manager.roomNumberUnique("26", error));
}

void TestPasoDB::testCourseCodeUnique() {
    DBManager manager(dbName);
    QSqlError error;
    // This course code is from in_memory.sql script so it should exist.
    QVERIFY(!manager.courseCodeUnique("IR3SP", error));
    QVERIFY(manager.courseCodeUnique("IR3BP", error));
}

void TestPasoDB::testCourseImport() {
    DBManager manager(dbName);
    QSqlError error;
    QString csvLine = "IR3XY Osnove X sa Y";
    QCOMPARE(manager.importCourse(csvLine, error),
             CourseImportError::INVALID_LINE);
    csvLine = "  , Osnove X sa Y";
    QCOMPARE(manager.importCourse(csvLine, error), CourseImportError::NO_CODE);
    csvLine = "IR3XY_IR3XY, Osnove X sa Y";
    QCOMPARE(manager.importCourse(csvLine, error),
             CourseImportError::CODE_TOO_LONG);
    csvLine = "IR3XY,";
    QCOMPARE(manager.importCourse(csvLine, error), CourseImportError::NO_NAME);
    csvLine = "IR3XY, "
              "1234567890123456789012345678901234567890123456789012345678901234"
              "567890";
    QCOMPARE(manager.importCourse(csvLine, error),
             CourseImportError::NAME_TOO_LONG);
    csvLine = "IR3XY, Osnove X";
    QCOMPARE(manager.importCourse(csvLine, error), CourseImportError::NO_ERROR);
    auto course = manager.getCourse("IR3XY", error);
    QCOMPARE(course->code(), QString("IR3XY"));
    QCOMPARE(course->name(), QString("Osnove X"));
    csvLine = "IR3XY,Osnove X sa Y";
    QCOMPARE(manager.importCourse(csvLine, error), CourseImportError::NO_ERROR);
    course = manager.getCourse("IR3XY", error);
    QCOMPARE(course->code(), QString("IR3XY"));
    QCOMPARE(course->name(), QString("Osnove X sa Y"));
}

QTEST_MAIN(TestPasoDB)
