#include "testpasodb.h"

#include "data.h"
#include "pasodb.h"

#include <QDataStream>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QUuid>
#include <list>

using namespace std;
using namespace paso::db;
using namespace paso::data;
using namespace paso::data::entity;

TestPasoDB::TestPasoDB() : TestBase() {}

void TestPasoDB::initTestCase() {
    TestBase::initTestCase();
    roomUUIDs << "d23a502b-a567-4929-ba99-9f93f36bf4e3";
    roomUUIDs << "7003528d-4c44-4f91-91b4-b82cb5afb009";
    usernames << "root"
              << "admin"
              << "room_manager"
              << "manager"
              << "scheduler";
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
    db.exec("DROP TABLE SYSTEM_USERS CASCADE");
}

void TestPasoDB::testGetAllSystemUsers() {
    DBManager manager(dbName);
    QSqlError error;
    QStringList users = usernames;
    auto systemUsers = manager.getAllSystemUsers(error);
    QCOMPARE(error.type(), QSqlError::NoError);
    QVERIFY(!systemUsers->empty());
    QCOMPARE(systemUsers->size(), size_t(5));
    for (const auto &user : *systemUsers) {
        users.removeOne(user.username());
    }
    QVERIFY2(users.empty(), "Not all users were returned!");
}

void TestPasoDB::testGetSystemUser() {
    DBManager manager(dbName);
    QSqlError error;
    auto systemUser = manager.getSystemUser("root", error);
    QCOMPARE(error.type(), QSqlError::NoError);
    QVERIFY((bool)systemUser);
    QCOMPARE(systemUser->username(), QString("root"));
    QCOMPARE(systemUser->password(), QString("root"));
    QCOMPARE(systemUser->firstName(), QString("System"));
    QCOMPARE(systemUser->lastName(), QString("Administrator"));
    QCOMPARE(systemUser->email(), QString("root@paso.system"));
    QCOMPARE(systemUser->role(), SystemRole::SUPER_USER);

    auto nonExistingUser = manager.getSystemUser("qrgerbeb", error);
    QCOMPARE(error.type(), QSqlError::NoError);
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
    QCOMPARE(user, *temp);
    user.setEmail("toptan@paso.system");
    user.setRole(SystemRole::ADMINISTRATOR);
    QVERIFY(manager.saveSystemUser(user, error));
    temp = manager.getSystemUser("toptan", error);
    QCOMPARE(user, *temp);
    temp = manager.getSystemUser("toptan", error);
    temp->setRole(SystemRole::ROOM_MANAGER);
    QVERIFY(manager.saveSystemUser(*temp, error));
    auto temp2 = manager.getSystemUser("toptan", error);
    QCOMPARE(*temp2, *temp);
    temp2->setRole(SystemRole::MANAGER);
    QVERIFY(manager.saveSystemUser(*temp2, error));
    temp = manager.getSystemUser("toptan", error);
    QCOMPARE(*temp2, *temp);
}

void TestPasoDB::testDeleteSystemUser() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    QVERIFY(!manager.deleteSystemUser("root", error));
    QCOMPARE(error.type(), QSqlError::NoError);
    QCOMPARE(manager.getAllSystemUsers(error)->size(), size_t(5));
    QVERIFY(manager.deleteSystemUser("room_manager", error));
    QCOMPARE(error.type(), QSqlError::NoError);
    QCOMPARE(manager.getAllSystemUsers(error)->size(), size_t(4));
    auto user = manager.getSystemUser("room_manager", error);
    QVERIFY(!user);
    db.exec("DROP TABLE SYSTEM_USER CASCADE");
    QVERIFY(!manager.deleteSystemUser("manager", error));
}

void TestPasoDB::testGetAllRooms() {
    DBManager manager(dbName);
    QSqlError error;
    auto rooms = manager.getAllRooms(error);
    QCOMPARE(error.type(), QSqlError::NoError);
    QVERIFY(!rooms->empty());
    QCOMPARE(rooms->size(), size_t(2));
    QStringList fetchedUUIDs;
    for (const auto &room : *rooms) {
        fetchedUUIDs << room.roomUUID();
    }
    QCOMPARE(fetchedUUIDs, roomUUIDs);
}

void TestPasoDB::testSaveRoom() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    auto roomUUID =
        QUuid::createUuid().toString().replace("{", "").replace("}", "");
    Room room(roomUUID, "Demo Room 1", "3");
    QVERIFY(manager.saveRoom(room, error));
    auto loadedRoom = manager.getRoom(roomUUID, error);
    QCOMPARE(room, *loadedRoom);
    loadedRoom->setName("Demo Lab 1");
    loadedRoom->setNumber("DL1");
    QVERIFY(manager.saveRoom(*loadedRoom, error));
    auto updatedRoom = manager.getRoom(roomUUID, error);
    QCOMPARE(*updatedRoom, *loadedRoom);
    db.exec("DROP TABLE ROOM CASCADE");
    roomUUID = QUuid::createUuid().toString().replace("{", "").replace("}", "");
    Room anotherRoom(roomUUID, "Demo Room 2", "DR2");
    QVERIFY(!manager.saveRoom(room, error));
}

void TestPasoDB::testGetRoom() {
    DBManager manager(dbName);
    QSqlError error;
    QUuid roomUUID(roomUUIDs[1]);
    auto room = manager.getRoom(roomUUID, error);
    QCOMPARE(error.type(), QSqlError::NoError);
    QVERIFY((bool)room);
    QCOMPARE(room->roomUUID(), roomUUIDs[1]);
    QCOMPARE(room->name(), QString("Paviljon 26"));
    QCOMPARE(room->number(), QString("P26"));
}

void TestPasoDB::testDeleteRoom() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    QUuid roomUUID(roomUUIDs[0]);
    QVERIFY(manager.deleteRoom(roomUUID, error));
    QCOMPARE(manager.getAllRooms(error)->size(), size_t(1));
    auto room = manager.getRoom(roomUUID, error);
    QVERIFY(!room);
    db.exec("DROP TABLE ROOM CASCADE");
    QVERIFY(!manager.deleteRoom(roomUUIDs[1], error));
}

void TestPasoDB::testSaveCourse() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    Course course;
    course.setCode("IR3BP1");
    course.setName("Baze podataka");
    QVERIFY(manager.saveCourse(course, error));
    auto loadedCourse = manager.getCourse("IR3BP1", error);
    QCOMPARE(course, *loadedCourse);
    loadedCourse->setName("Baze podataka 1");
    QVERIFY(manager.saveCourse(*loadedCourse, error));
    auto updatedCourse = manager.getCourse("IR3BP1", error);
    QCOMPARE(*updatedCourse, *loadedCourse);
    db.exec("DROP TABLE COURSE CASCADE");
    QVERIFY(!manager.saveCourse(*updatedCourse, error));
}

void TestPasoDB::testGetCourse() {
    DBManager manager(dbName);
    QSqlError error;
    auto course = manager.getCourse("IR3SP", error);
    QCOMPARE(error.type(), QSqlError::NoError);
    QVERIFY((bool)course);
    QCOMPARE(course->code(), QString("IR3SP"));
    QCOMPARE(course->name(), QString("Sistemsko programiranje"));
}

void TestPasoDB::testDeleteCourse() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    manager.importCourse("IR3XY, Osnove X sa Y", error);
    // Make sure that course exists.
    auto course = manager.getCourse("IR3SP", error);
    QVERIFY((bool)course);
    QVERIFY(manager.deleteCourse("IR3SP", error));
    // The course should not exist anymore.
    course = manager.getCourse("IR3SP", error);
    QVERIFY(!course);
    db.exec("DROP TABLE COURSE CASCADE");
    QVERIFY(!manager.deleteCourse("IR3XY", error));
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

void TestPasoDB::testIndexNumberUnique() {
    DBManager manager(dbName);
    QSqlError error;
    manager.importStudent("1996/0164, Tanaskovic,Toplica,toplica@gmail.com,4",
                          error);
    QVERIFY(!manager.indexNumberUnique("1996/0164", error));
    QVERIFY(manager.indexNumberUnique("111/11", error));
}

void TestPasoDB::testListNameUnique() {
    DBManager manager(dbName);
    QSqlError error;
    auto db = QSqlDatabase::database(dbName);
    db.exec("INSERT INTO LIST (NAME) VALUES ('List name 1')");
    QVERIFY(!manager.listNameUnique("List name 1", error));
    QVERIFY(manager.listNameUnique("List name 2", error));
}

void TestPasoDB::testSaveStudent() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    Student student("Pera", "Perić", "", "222/11", 2);
    QVERIFY(manager.saveStudent(student, error));
    QVERIFY(student.id() != 0);
    auto loadedStudent = manager.getStudentByIndexNumber("222/11", error);
    QCOMPARE(*loadedStudent, student);
    student.setFirstName("Jovan");
    student.setLastName("Jovanović");
    student.setEmail("jovan@jova.org");
    student.setRfid(QString());
    student.setIndexNumber("333/12");
    student.setYearOfStudy(3);
    QVERIFY(manager.saveStudent(student, error));
    auto updatedStudent = manager.getStudentByIndexNumber("333/12", error);
    QVERIFY(bool(updatedStudent));
    QCOMPARE(updatedStudent->firstName(), QString("Jovan"));
    QCOMPARE(updatedStudent->lastName(), QString("Jovanović"));
    QCOMPARE(updatedStudent->email(), QString("jovan@jova.org"));
    QVERIFY(updatedStudent->rfid().isNull());
    QCOMPARE(updatedStudent->indexNumber(), QString("333/12"));
    QCOMPARE(updatedStudent->yearOfStudy(), 3);
    updatedStudent->setRfid("RFID");
    updatedStudent->setEmail("");
    QVERIFY(manager.saveStudent(*updatedStudent, error));
    loadedStudent = manager.getStudentByIndexNumber("333/12", error);
    QCOMPARE(updatedStudent->rfid(), QString("RFID"));
    QCOMPARE(updatedStudent->email(), QString(""));
    Student anotherStudent("Mitar", "Mirić", "", "333/11", 2);
    db.exec("DROP TABLE STUDENT CASCADE");
    QVERIFY(!manager.saveStudent(anotherStudent, error));
    db.exec("create table student ("
            "        id              integer primary key,"
            "        index_number    text unique not null,"
            "        year_of_study   integer not null,"
            "        foreign key(id) references person(id) "
            "                        on delete cascade "
            "                        on update cascade);");
    db.exec("DROP TABLE PERSON CASCADE");
    QVERIFY(!manager.saveStudent(anotherStudent, error));
}

void TestPasoDB::testGetStudent() {
    DBManager manager(dbName);
    QSqlError error;
    Student temp("Petar", "Petrović", "petar@petrovic.com", "2001/2001", 5, 0,
                 "RRFFIIDD");
    manager.saveStudent(temp, error);
    auto student = manager.getStudentByIndexNumber("2001/2001", error);
    QCOMPARE(error.type(), QSqlError::NoError);
    QVERIFY((bool)student);
    QCOMPARE(student->firstName(), QString("Petar"));
    QCOMPARE(student->lastName(), QString("Petrović"));
    QCOMPARE(student->email(), QString("petar@petrovic.com"));
    QCOMPARE(student->rfid(), QString("RRFFIIDD"));
    QCOMPARE(student->indexNumber(), QString("2001/2001"));
    QCOMPARE(student->yearOfStudy(), 5);
    student = manager.getStudentByIndexNumber("XXX/YY", error);
    QCOMPARE(error.type(), QSqlError::NoError);
    QVERIFY(!(bool)student);
}

void TestPasoDB::testDeleteStudent() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    Student student("John", "Doe", "j.d@b.com", "555/15", 4, 0, "RRFFID");
    QVERIFY(manager.saveStudent(student, error));
    auto studentFromDB = manager.getStudentByIndexNumber("555/15", error);
    QVERIFY((bool)studentFromDB);
    QVERIFY(manager.deleteStudent("555/15", error));
    studentFromDB = manager.getStudentByIndexNumber("555/15", error);
    QVERIFY(!(bool)studentFromDB);
    db.exec("DROP TABLE STUDENT CASCADE");
    QVERIFY(!manager.deleteStudent("XXX/YY", error));
}

void TestPasoDB::testStudentImport() {
    DBManager manager(dbName);
    QSqlError error;
    QString csvLine = "164/96 Toplica Tanaskovic";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::INVALID_LINE);
    csvLine = ",Tanaskovic, Toplica,,";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::NO_INDEX_NUMBER);
    csvLine = "164/96,,Toplica,,";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::NO_LAST_NAME);
    csvLine = "164/96,TanaskovicTanaskovicTanaskovicTanaskovic,Toplica,,";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::LAST_NAME_TOO_LONG);
    csvLine = "164/96,Tanaskovic,,,";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::NO_FIRST_NAME);
    csvLine = "164/96,Tanaskovic,ToplicaToplicaToplicaToplicaToplicaToplica,,";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::FIRST_NAME_TOO_LONG);
    csvLine = "164/96,Tanaskovic,Toplica,,";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::NO_YEAR_OF_STUDY);
    csvLine = "164/96,Tanaskovic,Toplica,toplica<at>gmail.com,4";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::BAD_EMAIL);
    csvLine = "164/96,Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::BAD_INDEX_NUMBER);
    csvLine = "11996/164, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::BAD_INDEX_NUMBER);
    csvLine = "1986/0164, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::BAD_INDEX_NUMBER);
    csvLine = "2986/0164, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::BAD_INDEX_NUMBER);
    csvLine = "1996/0000, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::BAD_INDEX_NUMBER);
    csvLine = "1996/0164, Tanaskovic,Toplica,toplica@gmail.com,cetvrta";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::BAD_YEAR_OF_STUDY);
    csvLine = "1996/0164, Tanaskovic,Toplica,toplica@gmail.com,15";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::BAD_YEAR_OF_STUDY);
    csvLine = "1996/0164, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::NO_ERROR);
    auto student = manager.getStudentByIndexNumber("1996/0164", error);
    QCOMPARE(student->indexNumber(), QString("1996/0164"));
    QCOMPARE(student->lastName(), QString("Tanaskovic"));
    QCOMPARE(student->firstName(), QString("Toplica"));
    QCOMPARE(student->email(), QString("toplica@gmail.com"));
    QCOMPARE(student->yearOfStudy(), 4);
    csvLine = "1996/0164,Танасковић,\"Топлица\",toptan@etf.rs,5";
    QCOMPARE(manager.importStudent(csvLine, error),
             StudentImportError::NO_ERROR);
    student = manager.getStudentByIndexNumber("1996/0164", error);
    QCOMPARE(student->indexNumber(), QString("1996/0164"));
    QCOMPARE(student->lastName(), QString("Танасковић"));
    QCOMPARE(student->firstName(), QString("Топлица"));
    QCOMPARE(student->email(), QString("toptan@etf.rs"));
    QCOMPARE(student->yearOfStudy(), 5);
}

void TestPasoDB::testImportCourseStudent() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DELETE FROM PERSON");
    DBManager manager(dbName);
    QSqlError error;
    manager.importStudent("1996/0164, Tanaskovic,Toplica,toplica@gmail.com,4",
                          error);
    manager.importStudent("2001/2001, Petrovic,petar,pera@gmail.com,2", error);
    const QString courseCode = "IR3SP";
    QString csvLine = ",Tanaskovic, Toplica,,";
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "164/96 Toplica Tanaskovic";
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "164/96,,Toplica,,";
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "164/96,Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "11996/164, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "1986/0164, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "2986/0164, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "1996/0000, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "1996/0164, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::NO_ERROR);
    csvLine = "2001/2001";
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::NO_ERROR);
    csvLine = "2002/2002";
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::NON_EXISTING_STUDENT);

    auto courseStudents = manager.getCourseStudents(courseCode, error);
    QCOMPARE(courseStudents->size(), size_t(2));
    QStringList indexNumbers{"1996/0164", "2001/2001"};
    for (auto student : *courseStudents) {
        indexNumbers.removeOne(student.indexNumber());
    }
    QVERIFY(indexNumbers.empty());
    db.exec("DROP TABLE ENLISTED CASCADE");
    csvLine = "2001/2001";
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::DB_ERROR);
    db.exec("DROP TABLE STUDENT CASCADE");
    QCOMPARE(manager.importCourseStudent(courseCode, csvLine, error),
             ListStudentImportError::DB_ERROR);
}

void TestPasoDB::testImportListStudent() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    db.exec("DELETE FROM PERSON");
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
            "          VALUES('L1', 'false', 'true')");
    const auto listId = manager.getList("L1", error)->id();
    manager.importStudent("1996/0164, Tanaskovic,Toplica,toplica@gmail.com,4",
                          error);
    manager.importStudent("2001/2001, Petrovic,petar,pera@gmail.com,2", error);
    QString csvLine = ",Tanaskovic, Toplica,,";
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "164/96 Toplica Tanaskovic";
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "164/96,,Toplica,,";
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "164/96,Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "11996/164, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "1986/0164, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "2986/0164, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "1996/0000, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::BAD_INDEX_NUMBER);
    csvLine = "1996/0164, Tanaskovic,Toplica,toplica@gmail.com,4";
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::NO_ERROR);
    csvLine = "2001/2001";
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::NO_ERROR);
    csvLine = "2002/2002";
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::NON_EXISTING_STUDENT);

    auto members = manager.membersOfTheList(listId, error);
    QCOMPARE(members.size(), size_t(2));
    QStringList indexNumbers{"1996/0164", "2001/2001"};
    for (auto student : members) {
        indexNumbers.removeOne(student->value("INDEX_NUMBER").toString());
    }
    QVERIFY(indexNumbers.empty());
    db.exec("DROP TABLE MEMBER CASCADE");
    csvLine = "2001/2001";
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::DB_ERROR);
    db.exec("DROP TABLE STUDENT CASCADE");
    QCOMPARE(manager.importListStudent(listId, csvLine, error),
             ListStudentImportError::DB_ERROR);
}

void TestPasoDB::testEnlistingStudentsToCourse() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QStringList indexNumbers{"2001/2001", "2002/2002"};
    QSqlError error;
    manager.importStudent("2001/2001,Jovan, Jovanovic,,2", error);
    manager.importStudent("2002/2002,Milan,Milanovic,,2", error);
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    QVERIFY(manager.enlistStudentsToCourse("IR3SP", indexNumbers, error));
    auto students = manager.getCourseStudents("IR3SP", error);
    QCOMPARE(students->size(), size_t(2));
    for (const auto &student : *students) {
        indexNumbers.removeOne(student.indexNumber());
    }
    QVERIFY(indexNumbers.empty());
    indexNumbers.push_back("2002/2002");
    indexNumbers.push_back("2003/2003");
    QVERIFY(manager.enlistStudentsToCourse("IR3SP", indexNumbers, error));
    students = manager.getCourseStudents("IR3SP", error);
    QCOMPARE(students->size(), size_t(3));
    for (const auto &student : *students) {
        indexNumbers.removeOne(student.indexNumber());
    }
    QVERIFY(indexNumbers.empty());
    db.exec("DROP TABLE ENLISTED CASCADE");
    QVERIFY(!manager.enlistStudentsToCourse("IR3SP", {"2003/2003"}, error));
}

void TestPasoDB::testEnlistingStudentToCourses() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QStringList courseCodes{"IR3AA", "IR3AB"};
    QSqlError error;
    manager.importStudent("2001/2001,Jovan, Jovanovic,,2", error);
    manager.importStudent("2002/2002,Milan,Milanovic,,2", error);
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    manager.importCourse("IR3AA, AA course", error);
    manager.importCourse("IR3AB, AB course", error);
    manager.importCourse("IR3AC, AC course", error);
    QVERIFY(manager.enlistStudentToCourses("2003/2003", courseCodes, error));
    auto courses = manager.getStudentCourses("2003/2003", error);
    QCOMPARE(courses.size(), size_t(2));
    for (const auto &course : courses) {
        courseCodes.removeOne(course->value("CODE").toString());
    }
    QVERIFY(courseCodes.empty());
    QStringList notEnlisted{"2001/2001", "2002/2002"};
    auto enlistedStudents = manager.studentsEnlistedToCourse("IR3AA", error);
    auto notEnlistedStudents =
        manager.studentsNotEnlistedToCourse("IR3AA", error);
    QCOMPARE(enlistedStudents.size(), static_cast<size_t>(1));
    QCOMPARE(enlistedStudents[0]->value("INDEX_NUMBER").toString(),
             QString("2003/2003"));
    QCOMPARE(notEnlistedStudents.size(), static_cast<size_t>(2));
    for (auto entity : notEnlistedStudents) {
        notEnlisted.removeOne(entity->value("INDEX_NUMBER").toString());
    }
    QVERIFY(notEnlisted.empty());

    enlistedStudents = manager.studentsEnlistedToCourse("IR3AB", error);
    notEnlistedStudents = manager.studentsNotEnlistedToCourse("IR3AB", error);
    notEnlisted.push_back("2001/2001");
    notEnlisted.push_back("2002/2002");

    QCOMPARE(enlistedStudents.size(), static_cast<size_t>(1));
    QCOMPARE(enlistedStudents[0]->value("INDEX_NUMBER").toString(),
             QString("2003/2003"));
    QCOMPARE(notEnlistedStudents.size(), static_cast<size_t>(2));
    for (auto entity : notEnlistedStudents) {
        notEnlisted.removeOne(entity->value("INDEX_NUMBER").toString());
    }
    QVERIFY(notEnlisted.empty());

    courseCodes.push_back("IR3AA");
    courseCodes.push_back("IR3AC");
    QVERIFY(manager.enlistStudentToCourses("2003/2003", courseCodes, error));
    courses = manager.getStudentCourses("2003/2003", error);
    QCOMPARE(courses.size(), size_t(3));
    for (const auto &course : courses) {
        courseCodes.removeOne(course->value("CODE").toString());
    }
    QVERIFY(courseCodes.empty());
    db.exec("DROP TABLE ENLISTED CASCADE");
    QVERIFY(!manager.enlistStudentToCourses("2003/2003", {"IR3AB"}, error));
}

void TestPasoDB::testRemovingStudentFromCourses() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QStringList courseCodes{"IR3AA", "IR3AB"};
    QSqlError error;
    manager.importStudent("2001/2001,Jovan, Jovanovic,,2", error);
    manager.importStudent("2002/2002,Milan,Milanovic,,2", error);
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    manager.importCourse("IR3AA, AA course", error);
    manager.importCourse("IR3AB, AB course", error);
    manager.importCourse("IR3AC, AC course", error);
    manager.enlistStudentToCourses("2003/2003", courseCodes, error);
    QVERIFY(manager.removeStudentFromCourses("2003/2003", courseCodes, error));
    QVERIFY(manager.studentsEnlistedToCourse("IR3AA", error).empty());
    QVERIFY(manager.studentsEnlistedToCourse("IR3AB", error).empty());
    QVERIFY(manager.getStudentCourses("2003/2003", error).empty());
    db.exec("DROP TABLE ENLISTED CASCADE");
    QVERIFY(!manager.removeStudentFromCourses("2003/2003", {"IR3AB"}, error));
}

void TestPasoDB::testRemovingStudentsFromCourse() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QStringList indexNumbers{"2001/2001", "2002/2002"};
    QSqlError error;
    manager.importStudent("2001/2001,Jovan, Jovanovic,,2", error);
    manager.importStudent("2002/2002,Milan,Milanovic,,2", error);
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    manager.importCourse("IR3AA, AA course", error);
    manager.importCourse("IR3AB, AB course", error);
    manager.importCourse("IR3AC, AC course", error);
    manager.enlistStudentsToCourse("IR3AB", indexNumbers, error);
    QVERIFY(manager.removeStudentsFromCourse("IR3AB", indexNumbers, error));
    QVERIFY(manager.getCourseStudents("IR3AB", error)->empty());
    db.exec("DROP TABLE ENLISTED CASCADE");
    QVERIFY(!manager.removeStudentsFromCourse("IR3AB", {"2001/2001"}, error));
}

void TestPasoDB::testRemovingAllStudentsFromCourse() {
    DBManager manager(dbName);
    QStringList indexNumbers{"2001/2001", "2002/2002"};
    QSqlError error;
    manager.importStudent("2001/2001,Jovan, Jovanovic,,2", error);
    manager.importStudent("2002/2002,Milan,Milanovic,,2", error);
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    manager.importCourse("IR3AA, AA course", error);
    manager.importCourse("IR3AB, AB course", error);
    manager.importCourse("IR3AC, AC course", error);
    manager.enlistStudentsToCourse("IR3AB", indexNumbers, error);
    manager.beginTransaction();
    QVERIFY(manager.removeAllStudentsFromCourse("IR3AB", error));
    manager.commit();
    QVERIFY(manager.getCourseStudents("IR3AB", error)->empty());
}

void TestPasoDB::testUpdatingCourseStudents() {
    DBManager manager(dbName);
    QStringList addIndexNumbers{"2001/2001", "2002/2002"};
    QStringList removeIndexNumbers{"2003/2003", "2004/2004"};
    QSqlError error;
    manager.importStudent("2001/2001,Jovan, Jovanovic,,2", error);
    manager.importStudent("2002/2002,Milan,Milanovic,,2", error);
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    manager.importStudent("2004/2004,Bojana, Bojanic,,2", error);
    manager.importCourse("IR3AA, AA course", error);
    manager.importCourse("IR3AB, AB course", error);
    manager.importCourse("IR3AC, AC course", error);

    // S: {}
    manager.updateCourseStudents("IR3AB", QStringList(), QStringList(), error);
    QVERIFY(manager.getCourseStudents("IR3AB", error)->empty());

    // S: {}
    manager.updateCourseStudents("IR3AB", QStringList(), removeIndexNumbers,
                                 error);
    QVERIFY(manager.getCourseStudents("IR3AB", error)->empty());

    // S: {"2001/2001", "2002/2002"}
    manager.updateCourseStudents("IR3AB", addIndexNumbers, QStringList(),
                                 error);
    auto courseStudents = manager.getCourseStudents("IR3AB", error);
    QCOMPARE(courseStudents->size(), size_t(2));
    QStringList temp(addIndexNumbers);
    for (const auto &student : *courseStudents) {
        temp.removeOne(student.indexNumber());
    }
    QVERIFY(temp.empty());

    // S: {}
    manager.updateCourseStudents("IR3AB", QStringList(), addIndexNumbers,
                                 error);
    QVERIFY(manager.getCourseStudents("IR3AB", error)->empty());

    // S: {"2001/2001", "2002/2002"}
    manager.updateCourseStudents("IR3AB", addIndexNumbers, removeIndexNumbers,
                                 error);
    courseStudents = manager.getCourseStudents("IR3AB", error);
    QCOMPARE(courseStudents->size(), size_t(2));
    temp = addIndexNumbers;
    for (const auto &student : *courseStudents) {
        temp.removeOne(student.indexNumber());
    }
    QVERIFY(temp.empty());

    // S: {"2003/2003", "2004/2004"}
    manager.updateCourseStudents("IR3AB", removeIndexNumbers, addIndexNumbers,
                                 error);
    courseStudents = manager.getCourseStudents("IR3AB", error);
    QCOMPARE(courseStudents->size(), size_t(2));
    temp = removeIndexNumbers;
    for (const auto &student : *courseStudents) {
        temp.removeOne(student.indexNumber());
    }
    QVERIFY(temp.empty());

    // S: {"2001/2001", "2004/2004"}
    manager.updateCourseStudents("IR3AB", QStringList{"2001/2001"},
                                 QStringList{"2003/2003"}, error);
    courseStudents = manager.getCourseStudents("IR3AB", error);
    QCOMPARE(courseStudents->size(), size_t(2));
    temp = QStringList{"2001/2001", "2004/2004"};
    for (const auto &student : *courseStudents) {
        temp.removeOne(student.indexNumber());
    }
    QVERIFY(temp.empty());

    // S: {"2004/2004"}
    manager.updateCourseStudents("IR3AB", QStringList(),
                                 QStringList{"2001/2001"}, error);
    courseStudents = manager.getCourseStudents("IR3AB", error);
    QCOMPARE(courseStudents->size(), size_t(1));
    QCOMPARE((*courseStudents)[0].indexNumber(), QString("2004/2004"));

    // S: {"2002/2002", "2004/2004"}
    manager.updateCourseStudents("IR3AB", QStringList{"2002/2002"},
                                 QStringList(), error);
    courseStudents = manager.getCourseStudents("IR3AB", error);
    QCOMPARE(courseStudents->size(), size_t(2));
    temp = QStringList{"2002/2002", "2004/2004"};
    for (const auto &student : *courseStudents) {
        temp.removeOne(student.indexNumber());
    }
    QVERIFY(temp.empty());
}

void TestPasoDB::testAddingStudentsToTheList() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    QStringList indexNumbers{"2001/2001", "2002/2002"};
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
            "          VALUES('L1', 'false', 'true')");
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
            "          VALUES('L2', 'false', 'false')");
    manager.importStudent("2001/2001,Jovan, Jovanovic,,2", error);
    manager.importStudent("2002/2002,Milan,Milanovic,,2", error);
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    auto list = manager.getList("L1", error);
    QVERIFY(manager.addStudentsToList(list->id(), indexNumbers, error));
    auto students = manager.membersOfTheList(list->id(), error);
    QCOMPARE(students.size(), size_t(2));
    for (const auto &student : students) {
        indexNumbers.removeOne(student->value("INDEX_NUMBER").toString());
    }
    QVERIFY(indexNumbers.empty());
    indexNumbers.push_back("2002/2002");
    indexNumbers.push_back("2003/2003");
    QVERIFY(manager.addStudentsToList(list->id(), indexNumbers, error));
    students = manager.membersOfTheList(list->id(), error);
    QCOMPARE(students.size(), size_t(3));
    for (const auto &student : students) {
        indexNumbers.removeOne(student->value("INDEX_NUMBER").toString());
    }
    QVERIFY(indexNumbers.empty());
    db.exec("DROP TABLE MEMBER CASCADE");
    QVERIFY(!manager.addStudentsToList(list->id(), {"2002/2002"}, error));
}

void TestPasoDB::testRemovingStudentsFromTheList() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    QStringList indexNumbers{"2001/2001", "2002/2002"};
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
            "          VALUES('L1', 'false', 'true')");
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
            "          VALUES('L2', 'false', 'false')");
    manager.importStudent("2001/2001,Jovan, Jovanovic,,2", error);
    manager.importStudent("2002/2002,Milan,Milanovic,,2", error);
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    auto list = manager.getList("L1", error);
    manager.addStudentsToList(list->id(), indexNumbers, error);
    QVERIFY(manager.removeStudentsFromList(list->id(), indexNumbers, error));
    QVERIFY(manager.membersOfTheList(list->id(), error).empty());
    db.exec("DROP TABLE MEMBER CASCADE");
    QVERIFY(!manager.removeStudentsFromList(list->id(), {"2002/2002"}, error));
}

void TestPasoDB::testGetList() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    db.exec("DELETE FROM LIST");
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
            "          VALUES('L1', 'false', 'true')");
    auto list = manager.getList("L1", error);
    QCOMPARE(error.type(), QSqlError::NoError);
    QVERIFY((bool)list);
    QCOMPARE(list->name(), QString("L1"));
    QVERIFY(!list->system());
    QVERIFY(list->permanent());
    QVERIFY(list->expiryDate().isNull());

    list = manager.getList("L2", error);
    QVERIFY(!(bool)list);
}

void TestPasoDB::testUpdatingListStudents() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QStringList addIndexNumbers{"2001/2001", "2002/2002"};
    QStringList removeIndexNumbers{"2003/2003", "2004/2004"};
    QSqlError error;
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
            "          VALUES('L1', 'false', 'true')");
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
            "          VALUES('L2', 'false', 'false')");
    manager.importStudent("2001/2001,Jovan, Jovanovic,,2", error);
    manager.importStudent("2002/2002,Milan,Milanovic,,2", error);
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    manager.importStudent("2004/2004,Bojana, Bojanic,,2", error);
    auto listId = manager.getList("L1", error)->id();
    // S: {}
    manager.updateListStudents(listId, QStringList(), QStringList(), error);
    QVERIFY(manager.membersOfTheList(listId, error).empty());

    // S: {}
    manager.updateListStudents(listId, QStringList(), removeIndexNumbers,
                               error);
    QVERIFY(manager.membersOfTheList(listId, error).empty());

    // S: {"2001/2001", "2002/2002"}
    manager.updateListStudents(listId, addIndexNumbers, QStringList(), error);
    auto members = manager.membersOfTheList(listId, error);
    QCOMPARE(members.size(), size_t(2));
    QStringList temp(addIndexNumbers);
    for (const auto &student : members) {
        temp.removeOne(student->value("INDEX_NUMBER").toString());
    }
    QVERIFY(temp.empty());

    // S: {}
    manager.updateListStudents(listId, QStringList(), addIndexNumbers, error);
    QVERIFY(manager.membersOfTheList(listId, error).empty());

    // S: {"2001/2001", "2002/2002"}
    manager.updateListStudents(listId, addIndexNumbers, removeIndexNumbers,
                               error);
    members = manager.membersOfTheList(listId, error);
    QCOMPARE(members.size(), size_t(2));
    temp = addIndexNumbers;
    for (const auto &student : members) {
        temp.removeOne(student->value("INDEX_NUMBER").toString());
    }
    QVERIFY(temp.empty());

    // S: {"2003/2003", "2004/2004"}
    manager.updateListStudents(listId, removeIndexNumbers, addIndexNumbers,
                               error);
    members = manager.membersOfTheList(listId, error);
    QCOMPARE(members.size(), size_t(2));
    temp = removeIndexNumbers;
    for (const auto &student : members) {
        temp.removeOne(student->value("INDEX_NUMBER").toString());
    }
    QVERIFY(temp.empty());

    // S: {"2001/2001", "2004/2004"}
    manager.updateListStudents(listId, QStringList{"2001/2001"},
                               QStringList{"2003/2003"}, error);
    members = manager.membersOfTheList(listId, error);
    QCOMPARE(members.size(), size_t(2));
    temp = QStringList{"2001/2001", "2004/2004"};
    for (const auto &student : members) {
        temp.removeOne(student->value("INDEX_NUMBER").toString());
    }
    QVERIFY(temp.empty());

    // S: {"2004/2004"}
    manager.updateListStudents(listId, QStringList(), QStringList{"2001/2001"},
                               error);
    members = manager.membersOfTheList(listId, error);
    QCOMPARE(members.size(), size_t(1));
    QCOMPARE(members[0]->value("INDEX_NUMBER").toString(),
             QString("2004/2004"));

    // S: {"2002/2002", "2004/2004"}
    manager.updateListStudents(listId, QStringList{"2002/2002"}, QStringList(),
                               error);
    members = manager.membersOfTheList(listId, error);
    QCOMPARE(members.size(), size_t(2));
    temp = QStringList{"2002/2002", "2004/2004"};
    for (const auto &student : members) {
        temp.removeOne(student->value("INDEX_NUMBER").toString());
    }
    QVERIFY(temp.empty());
    db.exec("DROP TABLE MEMBER CASCADE");
    QVERIFY(!manager.updateListStudents(
        listId, QStringList(), QStringList{"2002/2002", "2004/2004"}, error));
}

void TestPasoDB::testRemovingAllStudentsFromList() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QStringList indexNumbers{"2001/2001", "2002/2002"};
    QSqlError error;
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
            "          VALUES('L1', 'false', 'true')");
    auto list = manager.getList("L1", error);
    manager.importStudent("2001/2001,Jovan, Jovanovic,,2", error);
    manager.importStudent("2002/2002,Milan,Milanovic,,2", error);
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    manager.addStudentsToList(list->id(), indexNumbers, error);
    manager.beginTransaction();
    QVERIFY(manager.removeAllStudentsFromList(list->id(), error));
    manager.commit();
    QVERIFY(manager.membersOfTheList(list->id(), error).empty());
}

void TestPasoDB::testGetActivity() {
    auto db = QSqlDatabase::database(dbName);
    db.exec(
        "INSERT INTO ACTIVITY(ID, NAME, TYPE, SCHEDULE_TYPE, "
        "                     DURATION, START_DATE, FINISH_DATE)"
        "              VALUES(1, 'A1', 'EXAM', 'ONCE',"
        "                     '03:00:00.000', '2016-08-15', '2016-08-15');");
    DBManager manager(dbName);
    QSqlError error;
    auto activity = manager.getActivity(1, error);
    QCOMPARE(error.type(), QSqlError::NoError);
    QVERIFY((bool)activity);
    QCOMPARE(activity->name(), QString("A1"));
    QCOMPARE(activity->type(), ActivityType::EXAM);
    QCOMPARE(activity->scheduleType(), ActivityScheduleType::ONCE);
    QCOMPARE(activity->duration(), QTime(3, 0));
    QCOMPARE(activity->startDate(), QDate(2016, 8, 15));
    QCOMPARE(activity->finishDate(), QDate(2016, 8, 15));
    activity = manager.getActivity(2, error);
    QVERIFY(!(bool)activity);
}

void TestPasoDB::testAssociateListsWithActivity() {
    auto db = QSqlDatabase::database(dbName);
    db.exec(
        "INSERT INTO ACTIVITY(ID, NAME, TYPE, SCHEDULE_TYPE, "
        "                     DURATION, START_DATE, FINISH_DATE)"
        "              VALUES(1, 'A1', 'EXAM', 'ONCE',"
        "                     '03:00:00.000', '2016-08-15', '2016-08-15');");
    db.exec(
        "INSERT INTO ACTIVITY(ID, NAME, TYPE, SCHEDULE_TYPE, "
        "                     DURATION, START_DATE, FINISH_DATE, CAN_OVERLAP)"
        "              VALUES(2, 'A2', 'INDIVIDUAL_WORK', 'WEEK_DAYS',"
        "                     '01:30:00.000', '2016-09-01', '2016-09-30', "
        "true);");

    DBManager manager(dbName);
    QList<quint64> listIds{1, 2, 3};
    QSqlError error;
    QVERIFY(manager.setActivityLists(1, listIds, error));
    QCOMPARE(manager.activityLists(1, error).size(), size_t(3));
    QVERIFY(manager.setActivityLists(1, {3, 4}, error));
    QCOMPARE(manager.activityLists(1, error).size(), size_t(2));
    db.exec("DROP TABLE ACTIVITY_LISTS CASCADE");
    QVERIFY(!manager.setActivityLists(1, {1}, error));
}

void TestPasoDB::testAssociateRoomsWithActivity() {
    auto db = QSqlDatabase::database(dbName);
    db.exec(
        "INSERT INTO ACTIVITY(ID, NAME, TYPE, SCHEDULE_TYPE, "
        "                     DURATION, START_DATE, FINISH_DATE)"
        "              VALUES(1, 'A1', 'EXAM', 'ONCE',"
        "                     '03:00:00.000', '2016-08-15', '2016-08-15');");
    db.exec(
        "INSERT INTO ACTIVITY(ID, NAME, TYPE, SCHEDULE_TYPE, "
        "                     DURATION, START_DATE, FINISH_DATE, CAN_OVERLAP)"
        "              VALUES(2, 'A2', 'INDIVIDUAL_WORK', 'WEEK_DAYS',"
        "                     '01:30:00.000', '2016-09-01', '2016-09-30', "
        "true);");

    DBManager manager(dbName);
    QList<quint64> roomIds{1, 2};
    QSqlError error;
    QVERIFY(manager.setActivityRooms(1, roomIds, error));
    QCOMPARE(manager.activityRooms(1, error).size(), size_t(2));
    db.exec("DROP TABLE ACTIVITY_ROOMS CASCADE");
    QVERIFY(!manager.setActivityRooms(1, {1}, error));
}
