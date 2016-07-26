#include "testcourseadministration.h"

#include "course.h"
#include "courseeditorwidget.h"
#include "coursevalidator.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QSqlField>

using namespace paso::data::entity;
using namespace paso::widget;
using namespace paso::admin;

TestCourseAdministration::TestCourseAdministration()
    : QObject(), dbName("paso") {}

void TestCourseAdministration::initTestCase() {
    auto db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName(":memory:");
    db.open();
}

void TestCourseAdministration::init() {
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

void TestCourseAdministration::cleanup() {
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

void TestCourseAdministration::testCourseValidator() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("INSERT INTO COURSE(CODE, NAME) VALUES('XXXXX', 'YYYYY')");

    const FieldTypes fieldTypes = {{"code", FieldType::LineEdit},
                                   {"name", FieldType::LineEdit}};
    auto codeLineEdit = new QLineEdit();
    auto nameLineEdit = new QLineEdit();
    const FieldEditors fieldEditors = {{"code", codeLineEdit},
                                       {"name", nameLineEdit}};
    const QString title = "Invalid data entered";
    const QSqlRecord emptyRecord;
    QSqlRecord notEmptyRecord;
    notEmptyRecord.append(QSqlField("code", QVariant::String));
    notEmptyRecord.append(QSqlField("name", QVariant::String));

    CourseValidator validator(fieldTypes, fieldEditors, this);

    auto result = validator.validate(emptyRecord);
    QVERIFY((bool)result);
    QCOMPARE(result->editor, codeLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("The course code has to be provided."));
    codeLineEdit->setText("1234567890");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, codeLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The course code cannot exceed 8 characters."));
    codeLineEdit->setText("XXXXX");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, codeLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(
        result->text,
        QString("The course with entered code already exists in the system."));
}

void TestCourseAdministration::testCourseEditorWidget() {}

QTEST_MAIN(TestCourseAdministration)
