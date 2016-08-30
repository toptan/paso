#include "testbase.h"

#include <QDebug>
#include <QProcess>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <iostream>

int TestBase::m_executed = 0;
int TestBase::m_failed = 0;

void TestBase::printTestStats() {
    std::cout << "Combined totals: " << m_executed - m_failed
              << " tests passed, " << m_failed << " tests failed." << std::endl;
}

TestBase::TestBase() : QObject(), dbName("paso") {}

TestBase::~TestBase() {}

void TestBase::initTestCase() {}

void TestBase::cleanupTestCase() {}

void TestBase::init() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("set client_min_messages=WARNING;");
    QFile create_objects_sql("create_objects.sql");
    create_objects_sql.open(QIODevice::ReadOnly);
    QTextStream in(&create_objects_sql);
    QString sqlString = in.readAll();
    db.exec(sqlString);
    if (db.lastError().type() != QSqlError::NoError) {
        qWarning() << db.lastError();
    }
    db.exec("set client_min_messages=NOTICE;");
    db.exec("INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, "
            "LAST_NAME, EMAIL, ROLE) VALUES ( 'admin', 'admin_password', "
            "'Admin', 'Admin', 'admin@paso.system', 'ADMINISTRATOR');");
    if (db.lastError().type() != QSqlError::NoError) {
        qWarning() << "admin:" << db.lastError();
    }

    db.exec("INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, "
            "LAST_NAME, EMAIL, ROLE) VALUES ( 'room_manager', "
            "'room_manager_password', 'Room', 'Manager', "
            "'room.manager@paso.system', 'ROOM_MANAGER');");
    if (db.lastError().type() != QSqlError::NoError) {
        qWarning() << "room_manager:" << db.lastError();
    }

    db.exec("INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, "
            "LAST_NAME, EMAIL, ROLE) VALUES ( 'manager', 'manager_password', "
            "'Manager', 'Managerovic', 'manager@paso.system', 'MANAGER');");
    if (db.lastError().type() != QSqlError::NoError) {
        qWarning() << "manager:" << db.lastError();
    }

    db.exec("INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, "
            "LAST_NAME, EMAIL, ROLE) VALUES ( 'scheduler', "
            "'scheduler_password', 'Scheduler', 'Schedulerovic', "
            "'scheduler@paso.system', 'SCHEDULER');");
    if (db.lastError().type() != QSqlError::NoError) {
        qWarning() << "scheduler:" << db.lastError();
    }

    db.exec("INSERT INTO ROOM (ROOM_UUID, NAME, ROOM_NUMBER) VALUES ( "
            "'{d23a502b-a567-4929-ba99-9f93f36bf4e3}', 'Laboratorija 42', "
            "'42');");
    if (db.lastError().type() != QSqlError::NoError) {
        qWarning() << "42:" << db.lastError();
    }

    db.exec("INSERT INTO ROOM (ROOM_UUID, NAME, ROOM_NUMBER) VALUES ( "
            "'{7003528d-4c44-4f91-91b4-b82cb5afb009}', 'Paviljon 26', 'P26');");
    if (db.lastError().type() != QSqlError::NoError) {
        qWarning() << "P26:" << db.lastError();
    }

    db.exec("INSERT INTO COURSE (CODE, NAME) VALUES ( 'IR3SP', 'Sistemsko "
            "programiranje'); ");
    if (db.lastError().type() != QSqlError::NoError) {
        qWarning() << "IR3SP:" << db.lastError();
    }
    db.exec("set client_min_messages=NOTICE;");
}

void TestBase::cleanup() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("set client_min_messages=WARNING;");
    QFile drop_objects_sql("drop_objects.sql");
    drop_objects_sql.open(QIODevice::ReadOnly);
    QTextStream in(&drop_objects_sql);
    QString sqlString = in.readAll();
    db.exec(sqlString);
    if (db.lastError().type() != QSqlError::NoError) {
        qWarning() << db.lastError();
    }
    db.exec("set client_min_messages=NOTICE;");

    m_executed++;
    if (QTest::currentTestFailed()) {
        m_failed++;
    }
}
