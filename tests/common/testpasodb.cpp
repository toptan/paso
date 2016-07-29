#include "testpasodb.h"

#include "data.h"
#include "pasodb.h"

#include <QDataStream>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QUuid>

using namespace paso::db;
using namespace paso::data;
using namespace paso::data::entity;

TestPasoDB::TestPasoDB() : QObject(), dbName("paso") {}

void TestPasoDB::initTestCase() {
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
    QFile in_memory_sql("../in_memory.sql");
    in_memory_sql.open(QIODevice::ReadOnly);
    QTextStream in(&in_memory_sql);
    QString sqlString = in.readAll();
    sqlString.replace("\n", " ");
    QStringList commands = sqlString.split("--");
    for (const auto &command : commands) {
        if (command.trimmed().isEmpty()) {
            continue;
        }
        db.exec(command);
        if (db.lastError().type() != QSqlError::NoError) {
            qDebug() << db.lastError() << ": " << command;
        }
    }
    in_memory_sql.close();
}

void TestPasoDB::cleanup() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DROP VIEW ENLISTED_STUDENTS");
    db.exec("DROP VIEW LIST_MEMBERS");
    db.exec("DROP TABLE SYSTEM_USER");
    db.exec("DROP TABLE ENLISTED");
    db.exec("DROP TABLE MEMBER");
    db.exec("DROP TABLE LIST");
    db.exec("DROP TABLE COURSE");
    db.exec("DROP TABLE ROOM");
    db.exec("DROP TABLE STUDENT");
    db.exec("DROP TABLE PERSON");
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
    db.exec("DROP TABLE SYSTEM_USERS");
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
}

void TestPasoDB::testGetStudent() {
    DBManager manager(dbName);
    QSqlError error;
    Student temp("Petar", "Petrović", "petar@petrovic.com", "2001/2001", 5, 0,
                 "RRFFIIDD");
    manager.saveStudent(temp, error);
    auto student = manager.getStudentByIndexNumber("2001/2001", error);
    QVERIFY(error.type() == QSqlError::NoError);
    QVERIFY((bool)student);
    QCOMPARE(student->firstName(), QString("Petar"));
    QCOMPARE(student->lastName(), QString("Petrović"));
    QCOMPARE(student->email(), QString("petar@petrovic.com"));
    QCOMPARE(student->rfid(), QString("RRFFIIDD"));
    QCOMPARE(student->indexNumber(), QString("2001/2001"));
    QCOMPARE(student->yearOfStudy(), 5);
    student = manager.getStudentByIndexNumber("XXX/YY", error);
    QVERIFY(error.type() == QSqlError::NoError);
    QVERIFY(!(bool)student);
}

void TestPasoDB::testDeleteStudent() {
    DBManager manager(dbName);
    QSqlError error;
    Student student("John", "Doe", "j.d@b.com", "555/15", 4, 0, "RRFFID");
    QVERIFY(manager.saveStudent(student, error));
    auto studentFromDB = manager.getStudentByIndexNumber("555/15", error);
    QVERIFY((bool)studentFromDB);
    QVERIFY(manager.deleteStudent("555/15", error));
    studentFromDB = manager.getStudentByIndexNumber("555/15", error);
    QVERIFY(!(bool)studentFromDB);
    QVERIFY(manager.deleteStudent("XXX/YY", error));
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

void TestPasoDB::testEnlistingStudentsToCourse() {
    DBManager manager(dbName);
    QStringList indexeNumbers{"2001/2001", "2002/2002"};
    QSqlError error;
    manager.importStudent("2001/2001,Jovan, Jovanovic,,2", error);
    manager.importStudent("2002/2002,Milan,Milanovic,,2", error);
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    QVERIFY(manager.enlistStudentsToCourse("IR3SP", indexeNumbers, error));
    auto students = manager.getCourseStudents("IR3SP", error);
    QVERIFY(students->size() == 2);
    for (const auto &student : *students) {
        indexeNumbers.removeOne(student.indexNumber());
    }
    QVERIFY(indexeNumbers.empty());
    indexeNumbers.push_back("2002/2002");
    indexeNumbers.push_back("2003/2003");
    QVERIFY(manager.enlistStudentsToCourse("IR3SP", indexeNumbers, error));
    students = manager.getCourseStudents("IR3SP", error);
    QVERIFY(students->size() == 3);
    for (const auto &student : *students) {
        indexeNumbers.removeOne(student.indexNumber());
    }
    QVERIFY(indexeNumbers.empty());
}

void TestPasoDB::testEnlistingStudentToCourses() {
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
    QVERIFY(courses->size() == 2);
    for (const auto &course : *courses) {
        courseCodes.removeOne(course.code());
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
    QVERIFY(courses->size() == 3);
    for (const auto &course : *courses) {
        courseCodes.removeOne(course.code());
    }
    QVERIFY(courseCodes.empty());
}

void TestPasoDB::testRemovingStudentFromCourses() {
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
    QVERIFY(manager.getStudentCourses("2003/2003", error)->empty());
}

void TestPasoDB::testRemovingStudentsFromCourse() {
    DBManager manager(dbName);
    QStringList indexeNumbers{"2001/2001", "2002/2002"};
    QSqlError error;
    manager.importStudent("2001/2001,Jovan, Jovanovic,,2", error);
    manager.importStudent("2002/2002,Milan,Milanovic,,2", error);
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    manager.importCourse("IR3AA, AA course", error);
    manager.importCourse("IR3AB, AB course", error);
    manager.importCourse("IR3AC, AC course", error);
    manager.enlistStudentsToCourse("IR3AB", indexeNumbers, error);
    QVERIFY(manager.removeStudentsFromCourse("IR3AB", indexeNumbers, error));
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
