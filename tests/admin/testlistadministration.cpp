#include "testlistadministration.h"

#include "list.h"
#include "listeditorwidget.h"
#include "listvalidator.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QSqlField>
#include <QSqlError>

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
    auto db = QSqlDatabase::database(dbName);
    db.exec("INSERT INTO LIST(NAME) VALUES('List name 1')");

    FieldTypes fieldTypes{{"name", FieldType::LineEdit},
                          {"system", FieldType::ComboBox},
                          {"permanent", FieldType::ComboBox}};
    auto nameLineEdit = new QLineEdit();
    FieldEditors fieldEditors{{"name", nameLineEdit},
                              {"system", new QCheckBox()},
                              {"permanent", new QCheckBox()}};
    const QString title = "Invalid data entered";
    const QSqlRecord emptyRecord;
    QSqlRecord notEmptyRecord;
    notEmptyRecord.append(QSqlField("name", QVariant::String));

    ListValidator validator(fieldTypes, fieldEditors, this);

    auto result = validator.validate(emptyRecord);
    QVERIFY((bool)result);
    QCOMPARE(result->editor, nameLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("The list name has to be provided."));
    nameLineEdit->setText(
        "12345678901234567890123456789012345678901234567890123456789012345");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, nameLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The list name cannot exceed 64 characters."));
    nameLineEdit->setText("List name 1");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, nameLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(
        result->text,
        QString("The list with entered name already exists in the system."));
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, nameLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(
        result->text,
        QString("The list with entered name already exists in the system."));
    notEmptyRecord.setValue("name", "List name 1");
    result = validator.validate(notEmptyRecord);
    QVERIFY(!(bool)result);
    notEmptyRecord.setValue("name", "Old list name");
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, nameLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(
        result->text,
        QString("The list with entered name already exists in the system."));
    nameLineEdit->setText("List name 2");
    result = validator.validate(notEmptyRecord);
    QVERIFY(!(bool)result);
    notEmptyRecord.setValue("name", "List name 1");
    result = validator.validate(notEmptyRecord);
    QVERIFY(!(bool)result);
    db.close();
    result = validator.validate(notEmptyRecord);
    QVERIFY(result->editor == nullptr);
    QCOMPARE(result->title, QString("Critical error"));
    QCOMPARE(result->text,
             QString("There was an error working with the database."));
    QCOMPARE(result->icon, QMessageBox::Critical);
}

void TestListAdministration::testListEditorWidget() {
    FieldTypes fieldTypes{{"name", FieldType::LineEdit},
                          {"system", FieldType::CheckBox},
                          {"permanent", FieldType::CheckBox}};
    QVariantMap columnLabels = {
        {"name", "Name"}, {"system", "System"}, {"permanent", "Permanent"}};
    QSqlRecord record;
    record.append(QSqlField("id", QVariant::ULongLong));
    record.append(QSqlField("name", QVariant::String));
    record.append(QSqlField("permanent", QVariant::Bool));
    record.append(QSqlField("system", QVariant::Bool));
    ListEditorWidget editor(fieldTypes);
    editor.setupUi(columnLabels, record);
    editor.onEditNewRecord(record);
    QApplication::processEvents();
    auto nameEdit = dynamic_cast<QLineEdit *>(editor.fieldEditors()["name"]);
    auto permanentCheck =
        dynamic_cast<QCheckBox *>(editor.fieldEditors()["permanent"]);
    auto systemCheck =
        dynamic_cast<QCheckBox *>(editor.fieldEditors()["system"]);
    QVERIFY(!nameEdit->isReadOnly());
    QCOMPARE(nameEdit->maxLength(), 64);
    QVERIFY(permanentCheck->isEnabled());
    QVERIFY(!systemCheck->isEnabled());
}

QTEST_MAIN(TestListAdministration)
