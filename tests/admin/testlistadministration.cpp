#include "testlistadministration.h"

#include "list.h"

#include "listvalidator.h"

using namespace paso::data::entity;
using namespace paso::widget;
using namespace paso::admin;

TestListAdministration::TestListAdministration() : QObject(), dbName("paso") {}

void TestListAdministration::initTestCase() {
    auto db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName(":memory:");
    db.open();
}

void TestListAdministration::init() {
    auto db = QSqlDatabase::database(dbName);
    QFile in_memory_sql("../in_memory.sql");
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

void TestListAdministration::cleanup() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DROP VIEW ENLISTED_STUDENTS");
    db.exec("DROP VIEW LIST_MEMBERS");
    db.exec("DROP TABLE SYSTEM_USER");
    db.exec("DROP TABLE ENLISTED");
    db.exec("DROP TABLE MEMBER");
    db.exec("DROP TABLE COURSE");
    db.exec("DROP TABLE ROOM");
    db.exec("DROP TABLE STUDENT");
    db.exec("DROP TABLE PERSON");
}

void TestListAdministration::testListValidator() {
    FieldTypes fieldTypes{{"NAME", FieldType::LineEdit},
                          {"SYSTEM", FieldType::ComboBox},
                          {"PERMANENT", FieldType::ComboBox}};

    ListValidator validator(fieldTypes, FieldEditors{}, this);
    QVERIFY(validator.validate(QSqlRecord()) == nullptr);
}

QTEST_MAIN(TestListAdministration)
