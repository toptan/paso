#include "testroomadministration.h"

#include "room.h"
#include "roomeditorwidget.h"
#include "roomform.h"
#include "roomtablemodel.h"
#include "roomvalidator.h"

#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlError>
#include <QSqlField>
#include <QTableView>
#include <QTimer>
#include <QUuid>

using namespace paso::data::entity;
using namespace paso::widget;
using namespace paso::admin;

TestRoomAdministration::TestRoomAdministration() : TestBase() {}

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

    notEmptyRecord.setValue("room_uuid",
                            "{d23a502b-a567-4929-ba99-9f93f36bf4e3}");
    uuidLineEdit->setText("{d23a502b-a567-4929-ba99-9f93f36bf4e3}");
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, nameLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The name of the room cannot be left empty."));

    nameLineEdit->setText(
        "12345678901234567890123456789012345678901234567890123456789012345");
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, nameLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The name of the room cannot exceed 64 characters."));

    nameLineEdit->setText("Prostorija 2");
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, numberLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("The room number has to be provided."));

    numberLineEdit->setText("1234567890");
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, numberLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The room number cannot exceed 8 characters."));

    numberLineEdit->setText("42");
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, numberLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(
        result->text,
        QString("The room with entered number already exists in the system."));

    numberLineEdit->setText("142");
    result = validator.validate(notEmptyRecord);
    QVERIFY(!(bool)result);

    auto db = QSqlDatabase::database(dbName);
    db.exec("ALTER TABLE ROOM RENAME TO ROOM_X");
    db.exec("CREATE TABLE ROOM ( "
            "   ID          INTEGER PRIMARY KEY AUTOINCREMENT,"
            "   ROOM_UUID   TEXT UNIQUE NOT NULL,"
            "   NAME        TEXT NOT NULL)");
    db.exec("INSERT INTO ROOM (ID, ROOM_UUID, NAME)"
            "SELECT ID, ROOM_UUID, NAME FROM ROOM_X");
    uuidLineEdit->setText(QUuid::createUuid().toString());
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, numberLineEdit);
    QCOMPARE(result->title, QString("Critical error"));
    QCOMPARE(result->text,
             QString("There was an error working with the database."));
    QVERIFY(!result->detailedText.isEmpty());

    db.exec("DROP TABLE ROOM");
    db.exec("CREATE TABLE ROOM ( "
            "   ID          INTEGER PRIMARY KEY AUTOINCREMENT,"
            "   NAME        TEXT NOT NULL,"
            "   ROOM_NUMBER TEXT UNIQUE NOT NULL)");
    db.exec("INSERT INTO ROOM (ID, NAME, ROOM_NUMBER)"
            "SELECT ID, NAME, ROOM_NUMBER FROM ROOM_X");
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, uuidLineEdit);
    QCOMPARE(result->title, QString("Critical error"));
    QCOMPARE(result->text,
             QString("There was an error working with the database."));
    QVERIFY(!result->detailedText.isEmpty());

    for (auto editor : fieldEditors.values()) {
        delete editor;
    }
}

void TestRoomAdministration::testRoomEditorWidget() {
    FieldTypes fieldTypes{{"room_uuid", FieldType::LineEdit},
                          {"name", FieldType::LineEdit},
                          {"room_number", FieldType::LineEdit}};
    QVariantMap columnLabels{{"room_uuid", "UUID"},
                             {"name", "Name"},
                             {"room_number", "Room number"}};
    QSqlRecord record;
    record.append(QSqlField("id", QVariant::ULongLong));
    record.append(QSqlField("room_uuid", QVariant::String));
    record.append(QSqlField("name", QVariant::String));
    record.append(QSqlField("room_number", QVariant::String));
    RoomEditorWidget editor(fieldTypes);
    editor.setupUi(columnLabels, record);
    editor.onEditNewRecord(record);
    QApplication::processEvents();
    auto uuidEdit =
        dynamic_cast<QLineEdit *>(editor.fieldEditors()["room_uuid"]);
    auto nameEdit = dynamic_cast<QLineEdit *>(editor.fieldEditors()["name"]);
    auto numberEdit =
        dynamic_cast<QLineEdit *>(editor.fieldEditors()["room_number"]);
    QVERIFY(uuidEdit->isReadOnly());
    QVERIFY(!nameEdit->isReadOnly());
    QCOMPARE(nameEdit->maxLength(), 64);
    QVERIFY(!numberEdit->isReadOnly());
    QCOMPARE(numberEdit->maxLength(), 8);
    editor.onEditExistingRecord(record);
    QVERIFY(uuidEdit->isReadOnly());
    QVERIFY(!nameEdit->isReadOnly());
    QVERIFY(!numberEdit->isReadOnly());
}

void TestRoomAdministration::testRoomTableModel() {
    QVariantMap columnLabels{{"room_uuid", "Room UUID"},
                             {"name", "Name"},
                             {"room_number", "Room Number"}};
    RoomTableModel model(columnLabels, QSqlDatabase::database(dbName));
    QCOMPARE(model.columnCount(), 4);
    QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("id"));
    QCOMPARE(model.headerData(1, Qt::Horizontal).toString(),
             QString("Room UUID"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(), QString("Name"));
    QCOMPARE(model.headerData(3, Qt::Horizontal).toString(),
             QString("Room Number"));
}

void TestRoomAdministration::testRoomForm() {
    RoomForm form;
    form.show();
    QTest::qWaitForWindowExposed(&form);
    auto tableView = form.findChild<QTableView *>();
    auto roomEditorWidget = form.findChild<RoomEditorWidget *>();

    QVERIFY(tableView != nullptr);
    QVERIFY(roomEditorWidget != nullptr);

    tableView->selectRow(0);
    QApplication::processEvents();
    QAction *newAction = nullptr;
    QAction *deleteAction = nullptr;
    for (auto action : form.toolBarActions()) {
        QVERIFY(action->isEnabled());
        if (action->objectName() == "NEW_RECORD_ACTION") {
            newAction = action;
            continue;
        }
        if (action->objectName() == "DELETE_RECORD_ACTION") {
            deleteAction = action;
            continue;
        }
    }
    QVERIFY(newAction != nullptr);
    QVERIFY(deleteAction != nullptr);

    bool deleteMessageBoxShown = false;
    auto timerCallback = [&deleteMessageBoxShown]() {
        QMessageBox *msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::keyClick(msgBox, Qt::Key_Escape);
        deleteMessageBoxShown = true;
    };

    QTimer::singleShot(200, timerCallback);
    deleteAction->trigger();
    QApplication::processEvents();
    QVERIFY(deleteMessageBoxShown);

    auto oldRowCount = tableView->model()->rowCount();
    newAction->trigger();
    QApplication::processEvents();
    auto uuidEdit = dynamic_cast<QLineEdit *>(
        roomEditorWidget->fieldEditors()["room_uuid"]);
    auto nameEdit =
        dynamic_cast<QLineEdit *>(roomEditorWidget->fieldEditors()["name"]);
    auto numberEdit = dynamic_cast<QLineEdit *>(
        roomEditorWidget->fieldEditors()["room_number"]);
    auto saveButton = dynamic_cast<QDialogButtonBox *>(
                          roomEditorWidget->findChild<QDialogButtonBox *>())
                          ->button(QDialogButtonBox::Save);
    uuidEdit->setText(QUuid::createUuid().toString());
    nameEdit->setText("XXXXX");
    numberEdit->setText("YYYYY");
    saveButton->click();
    QApplication::processEvents();
    QCOMPARE(tableView->model()->rowCount(), oldRowCount + 1);
}
