#include "testpasodb.h"

#include "pasodb.h"
#include <QDebug>
#include <QSqlError>

using namespace paso::db;

void TestPasoDB::initTestCase() {
    auto db = QSqlDatabase::addDatabase("QSQLITE", "paso");
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
                    "'root',"
                    "'root',"
                    "'Root',"
                    "'Rootovic',"
                    "'root@paso.system',"
                    "'SUPER_USER')");
    db.exec("INSERT INTO SYSTEM_USER VALUES ("
                    "'admin',"
                    "'admin',"
                    "'Admin',"
                    "'Admin',"
                    "'admin@paso.system',"
                    "'ADMINISTRATOR')");
}

void TestPasoDB::testGetAllSystemUsers() {
    QSqlError error;
    auto systemUsers = getAllSystemUsers("paso", error);
    QVERIFY(error.type() == QSqlError::NoError);
    QVERIFY(!systemUsers->empty());
    QVERIFY(systemUsers->size() == 2);
    auto user = systemUsers->at(0);
    QVERIFY(user["USERNAME"] == "admin");
    user = systemUsers->at(1);
    QVERIFY(user["USERNAME"] == "root");
}

void TestPasoDB::testGetSystemUser() {
    QSqlError error;
    auto systemUser = getSystemUser("paso", "root", error);
    QVERIFY(error.type() == QSqlError::NoError);
    QVERIFY(!systemUser.empty());
    QVERIFY(systemUser["USERNAME"] == "root");
}
