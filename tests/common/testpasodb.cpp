#include "testpasodb.h"

#include "data.h"
#include "pasodb.h"

#include <QDebug>
#include <QSqlError>
#include <QUuid>

using namespace paso::db;
using namespace paso::data;

void TestPasoDB::initTestCase() {
    dbName = "paso";
    roomUUIDs << QUuid::createUuid().toString();
    roomUUIDs << QUuid::createUuid().toString();
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
    db.exec("CREATE TABLE SYSTEM_USER ("
            "USERNAME TEXT PRIMARY KEY,"
            "PASSWORD TEXT NOT NULL,"
            "FIRST_NAME TEXT,"
            "LAST_NAME TEXT,"
            "EMAIL TEXT,"
            "ROLE TEXT)");
    db.exec("CREATE TABLE ROOM ("
            "ROOM_UUID TEXT PRIMARY KEY,"
            "NAME TEXT NOT NULL,"
            "NUMBER TEXT NOT NULL)");

    // Inserting test data form system users.
    db.exec("INSERT INTO SYSTEM_USER VALUES ("
            "'admin',"
            "'admin_password',"
            "'Admin',"
            "'Admin',"
            "'admin@paso.system',"
            "'ADMINISTRATOR')");
    db.exec("INSERT INTO SYSTEM_USER VALUES ("
            "'room_manager',"
            "'room_manager_password',"
            "'Room',"
            "'Manager',"
            "'room.manager@paso.system',"
            "'ROOM_MANAGER')");
    db.exec("INSERT INTO SYSTEM_USER VALUES ("
            "'manager',"
            "'manager_password',"
            "'Manager',"
            "'Managerovic',"
            "'manager@paso.system',"
            "'MANAGER')");
    db.exec("INSERT INTO SYSTEM_USER VALUES ("
            "'scheduler',"
            "'scheduler_password',"
            "'Scheduler',"
            "'Schedulerovic',"
            "'scheduler@paso.system',"
            "'SCHEDULER')");

    // Inserting test data for rooms.
    db.exec("INSERT INTO ROOM VALUES ("
            "'" +
            roomUUIDs[0] + "',"
                           "'Laboratorija 42',"
                           "'42')");
    db.exec("INSERT INTO ROOM VALUES ("
            "'" +
            roomUUIDs[1] + "',"
                           "'Paviljon 26',"
                           "'P26')");
}

void TestPasoDB::cleanup() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DROP TABLE SYSTEM_USER");
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
    loadedRoom->setNumber("42");
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
