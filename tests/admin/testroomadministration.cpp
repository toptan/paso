#include "testroomadministration.h"

#include "room.h"
#include "roomeditorwidget.h"
#include "roomtablemodel.h"
#include "roomvalidator.h"

#include <QLineEdit>
#include <QSqlError>
#include <QSqlField>

using namespace paso::data::entity;
using namespace paso::widget;
using namespace paso::admin;

TestRoomAdministration::TestRoomAdministration() : QObject(), dbName("paso") {}

void TestRoomAdministration::initTestCase() {
    auto db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName(":memory:");
    db.open();
}

void TestRoomAdministration::init() {
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

void TestRoomAdministration::cleanup() {
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

void TestRoomAdministration::testRoomValidator() {
    FieldTypes fieldTypes{{"room_uuid", FieldType::LineEdit},
                          {"name", FieldType::LineEdit},
                          {"room_number", FieldType::LineEdit}};
    auto uuidLineEdit = new QLineEdit();
    auto nameLineEdit = new QLineEdit();
    auto numberLineEdit = new QLineEdit();
    FieldEditors fieldEditors{{"room_uuid", uuidLineEdit},
                              {"name", nameLineEdit},
                              {"room_number", numberLineEdit}};
    const QString title = "Invalid data entered";
    const QSqlRecord emptyRecord;
    QSqlRecord notEmptyRecord;
    notEmptyRecord.append(QSqlField("id", QVariant::ULongLong));
    notEmptyRecord.append(QSqlField("room_uuid", QVariant::String));
    notEmptyRecord.append(QSqlField("name", QVariant::String));
    notEmptyRecord.append(QSqlField("room_number", QVariant::String));

    RoomValidator validator(fieldTypes, fieldEditors, this);

    auto result = validator.validate(emptyRecord);
    QVERIFY((bool)result);
    QCOMPARE(result->editor, uuidLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("Room UUID has to be provided."));

    uuidLineEdit->setText("{d23a502b-a567-4929-ba99-9f93f36bf4e3}");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, uuidLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("The room UUID you entered is not unique."));

    notEmptyRecord.setValue("room_uuid", QUuid::createUuid().toString());
    uuidLineEdit->setText("{d23a502b-a567-4929-ba99-9f93f36bf4e3}");
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, uuidLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("The room UUID you entered is not unique."));



    for (auto editor : fieldEditors.values()) {
        delete editor;
    }
}

void TestRoomAdministration::testRoomEditorWidget() {}

void TestRoomAdministration::testRoomTableModel() {}

QTEST_MAIN(TestRoomAdministration)
