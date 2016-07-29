#include "testbase.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

TestBase::TestBase() : QObject(), dbName("paso") {}

void TestBase::initTestCase() {
    auto db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName(":memory:");
    db.open();
}

void TestBase::cleanupTestCase() { QSqlDatabase::removeDatabase(dbName); }

void TestBase::init() {
    auto db = QSqlDatabase::database(dbName);
    QFile in_memory_sql("in_memory.sql");
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

void TestBase::cleanup() {
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
