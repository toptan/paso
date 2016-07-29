#include "testcourseadministration.h"

#include "course.h"
#include "courseeditorwidget.h"
#include "coursetablemodel.h"
#include "coursevalidator.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QSqlError>
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

void TestCourseAdministration::cleanup() {
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

    codeLineEdit->setText("XXXXX");
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, codeLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(
        result->text,
        QString("The course with entered code already exists in the system."));

    notEmptyRecord.setValue("code", "XXXXX");
    nameLineEdit->setText("Course name x");
    result = validator.validate(notEmptyRecord);
    QVERIFY(!(bool)result);

    notEmptyRecord.setValue("code", "YYYYY");
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, codeLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(
        result->text,
        QString("The course with entered code already exists in the system."));

    codeLineEdit->setText("ZZZZZ");
    nameLineEdit->setText("");
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, nameLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The name of the course cannot be left empty."));

    nameLineEdit->setText(
        "12345678901234567890123456789012345678901234567890123456789012345");
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, nameLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The name of the course cannot exceed 64 characters."));

    db.close();
    result = validator.validate(notEmptyRecord);
    QVERIFY(result->editor == nullptr);
    QCOMPARE(result->title, QString("Critical error"));
    QCOMPARE(result->text,
             QString("There was an error working with the database."));
    QCOMPARE(result->icon, QMessageBox::Critical);

    for (auto editor : fieldEditors.values()) {
        delete editor;
    }
}

void TestCourseAdministration::testCourseEditorWidget() {
    FieldTypes fieldTypes{{"code", FieldType::LineEdit},
                          {"name", FieldType::LineEdit}};
    QVariantMap columnLabels{{"code", "Code"}, {"name", "Course"}};
    QSqlRecord record;
    record.append(QSqlField("id", QVariant::ULongLong));
    record.append(QSqlField("code", QVariant::String));
    record.append(QSqlField("name", QVariant::String));
    CourseEditorWidget editor(fieldTypes);
    editor.setupUi(columnLabels, record);
    editor.onEditNewRecord(record);
    QApplication::processEvents();
    auto codeEdit = dynamic_cast<QLineEdit *>(editor.fieldEditors()["code"]);
    auto nameEdit = dynamic_cast<QLineEdit *>(editor.fieldEditors()["name"]);
    QVERIFY(!codeEdit->isReadOnly());
    QCOMPARE(codeEdit->maxLength(), 8);
    QVERIFY(!nameEdit->isReadOnly());
    QCOMPARE(nameEdit->maxLength(), 64);
}

void TestCourseAdministration::testCourseTableModel() {
    QVariantMap columnLabels{{"CODE", "Code"}, {"NAME", "Course"}};
    CourseTableModel model(columnLabels, QSqlDatabase::database(dbName));
    QCOMPARE(model.columnCount(), 3);
    QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("ID"));
    QCOMPARE(model.headerData(1, Qt::Horizontal).toString(), QString("Code"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(), QString("Course"));
}
