#include "testpasodb.h"

#include "pasodb.h"
#include <QDebug>
#include <QSqlDatabase>

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
}

void TestPasoDB::testGetAllSystemUsers() {
  QVERIFY(!getAllSystemUsers("paso")->empty());
}
