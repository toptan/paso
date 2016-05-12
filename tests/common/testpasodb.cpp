#include "testpasodb.h"

#include "data.h"
#include "pasodb.h"
#include <QDebug>
#include <QSqlError>

using namespace paso::db;
using namespace paso::data;

void TestPasoDB::initTestCase() {
    dbName = "paso";
    auto db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName(":memory:");
    db.open();
    db.exec("CREATE TABLE SYSTEM_USER ("
            "USERNAME TEXT PRIMARY KEY,"
            "PASSWORD TEXT NOT NULL,"
            "FIRST_NAME TEXT,"
            "LAST_NAME TEXT,"
            "EMAIL TEXT,"
            "ROLE TEXT)");
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
}

void TestPasoDB::testGetAllSystemUsers() {
    DBManager manager(dbName);
    QSqlError error;
    QStringList users(
        {"admin", "root", "room_manager", "manager", "scheduler"});
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
