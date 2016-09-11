#include "testteacheradministration.h"

#include "entryreportdialog.h"
#include "teacher.h"
#include "teachereditorwidget.h"
#include "teacherform.h"
#include "teacherquerymodel.h"
#include "teachervalidator.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlError>
#include <QSqlField>
#include <QTableView>
#include <QTimer>

using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::widget;
using namespace paso::admin;
using namespace paso::model;

TestTeacherAdministration::TestTeacherAdministration() : TestBase() {}

void TestTeacherAdministration::testTeacherValidator() {
    const FieldTypes fieldTypes = {{"first_name", FieldType::LineEdit},
                                   {"last_name", FieldType::LineEdit},
                                   {"email", FieldType::LineEdit},
                                   {"rfid", FieldType::LineEdit},
                                   {"employee_number", FieldType::LineEdit},
                                   {"office", FieldType::LineEdit}};
    const QString title = "Invalid data entered";
    QSqlRecord emptyRecord;

    auto firstNameEdit = new QLineEdit();
    auto lastNameEdit = new QLineEdit();
    auto emailEdit = new QLineEdit();
    auto rfidEdit = new QLineEdit();
    auto employeeNumberEdit = new QLineEdit();
    auto officeEdit = new QLineEdit();
    const FieldEditors fieldEditors{{"first_name", firstNameEdit},
                                    {"last_name", lastNameEdit},
                                    {"email", emailEdit},
                                    {"rfid", rfidEdit},
                                    {"employee_number", employeeNumberEdit},
                                    {"office", officeEdit}};

    TeacherValidator validator(fieldTypes, fieldEditors, this);
    firstNameEdit->setText("Toplica");
    lastNameEdit->setText("Tanasković");
    emailEdit->setText("toplica@tanaskovic.com");

    auto result = validator.validate(emptyRecord);
    QVERIFY((bool)result);
    QCOMPARE(result->editor, employeeNumberEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The employee number cannot be left empty."));

    employeeNumberEdit->setText("123456");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, officeEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("The office number cannot be left empty."));

    officeEdit->setText("104");
    result = validator.validate(emptyRecord);
    QVERIFY(!(bool)result);

    Teacher t("Petar", "Petrovic", "pera@pera.com", "123456", "104");
    QSqlError error;
    DBManager manager(dbName);
    manager.saveTeacher(t, error);
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, employeeNumberEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(
        result->text,
        QString(
            "The teacher with employee number %1 already exists in the system.")
            .arg("123456"));

    auto db = QSqlDatabase::database(dbName);
    db.exec("DROP TABLE TEACHER CASCADE");
    firstNameEdit->setText("Toplica");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, employeeNumberEdit);
    QCOMPARE(result->title, QString("Critical error"));
    QCOMPARE(result->text,
             QString("There was an error working with the database."));
    QVERIFY(!result->detailedText.isEmpty());
}

void TestTeacherAdministration::testTeacherEditorWidget() {
    const FieldTypes fieldTypes{{"first_name", FieldType::LineEdit},
                                {"last_name", FieldType::LineEdit},
                                {"email", FieldType::LineEdit},
                                {"rfid", FieldType::LineEdit},
                                {"employee_number", FieldType::LineEdit},
                                {"office", FieldType::LineEdit}};

    const QVariantMap columnLabels = {
        {"first_name", QObject::tr("First Name")},
        {"last_name", QObject::tr("Last Name")},
        {"email", QObject::tr("Email")},
        {"rfid", QObject::tr("RFID")},
        {"employee_number", QObject::tr("Employee Number")},
        {"office", QObject::tr("Office Number")}};

    QSqlRecord record;
    record.append(QSqlField("id", QVariant::String));
    record.append(QSqlField("first_name", QVariant::String));
    record.append(QSqlField("last_name", QVariant::String));
    record.append(QSqlField("email", QVariant::String));
    record.append(QSqlField("rfid", QVariant::String));
    record.append(QSqlField("employee_number", QVariant::String));
    record.append(QSqlField("office", QVariant::String));

    TeacherEditorWidget editor(fieldTypes);
    editor.setupUi(columnLabels, record);
    editor.show();
    QTest::qWaitForWindowExposed(&editor);

    editor.onEditNewRecord(record);
    QApplication::processEvents();
    auto firstNameEdit =
        dynamic_cast<QLineEdit *>(editor.fieldEditors()["first_name"]);
    auto lastNameEdit =
        dynamic_cast<QLineEdit *>(editor.fieldEditors()["last_name"]);
    auto emailEdit = dynamic_cast<QLineEdit *>(editor.fieldEditors()["email"]);
    auto rfidEdit = dynamic_cast<QLineEdit *>(editor.fieldEditors()["rfid"]);
    auto employeeNumberEdit =
        dynamic_cast<QLineEdit *>(editor.fieldEditors()["employee_number"]);
    auto officeEdit =
        dynamic_cast<QLineEdit *>(editor.fieldEditors()["office"]);

    QVERIFY(!firstNameEdit->isReadOnly());
    QVERIFY(!lastNameEdit->isReadOnly());
    QVERIFY(!emailEdit->isReadOnly());
    QVERIFY(!rfidEdit->isReadOnly());
    QVERIFY(!employeeNumberEdit->isReadOnly());
    QVERIFY(!officeEdit->isReadOnly());

    QCOMPARE(firstNameEdit->maxLength(), 32);
    QCOMPARE(lastNameEdit->maxLength(), 32);
    QCOMPARE(emailEdit->maxLength(), 64);
    QCOMPARE(rfidEdit->maxLength(), 64);
    QCOMPARE(employeeNumberEdit->maxLength(), 16);
    QCOMPARE(officeEdit->maxLength(), 32);
}

void TestTeacherAdministration::testTeacherQueryModel() {
    const QVariantMap columnLabels = {
        {"first_name", QObject::tr("First Name")},
        {"last_name", QObject::tr("Last Name")},
        {"email", QObject::tr("Email")},
        {"rfid", QObject::tr("RFID")},
        {"employee_number", QObject::tr("Employee Number")},
        {"office", QObject::tr("Office Number")}};

    TeacherQueryModel model(columnLabels, QSqlDatabase::database(dbName), this);
    QCOMPARE(model.columnCount(), 7);
    QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("id"));
    QCOMPARE(model.headerData(1, Qt::Horizontal).toString(),
             QString("Employee Number"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(),
             QString("Last Name"));
    QCOMPARE(model.headerData(3, Qt::Horizontal).toString(),
             QString("First Name"));
    QCOMPARE(model.headerData(4, Qt::Horizontal).toString(), QString("Email"));
    QCOMPARE(model.headerData(5, Qt::Horizontal).toString(),
             QString("Office Number"));
    QCOMPARE(model.headerData(6, Qt::Horizontal).toString(), QString("RFID"));
}

void TestTeacherAdministration::testTeacherForm() {
    DBManager manager(dbName);
    QSqlError error;
    Teacher t("Petar", "Petrovic", "pera@pera.com", "123456", "104");
    manager.saveTeacher(t, error);

    TeacherForm form;
    form.show();
    QTest::qWaitForWindowExposed(&form);
    auto tableView = form.findChild<QTableView *>();
    auto editor = form.findChild<TeacherEditorWidget *>();

    QVERIFY(tableView != nullptr);
    QVERIFY(editor != nullptr);

    tableView->selectRow(0);
    QApplication::processEvents();
    QAction *newAction = nullptr;
    QAction *editAction = nullptr;
    QAction *deleteAction = nullptr;
    QAction *refreshAction = nullptr;
    QAction *reportAction = nullptr;
    for (auto action : form.toolBarActions()) {
        QVERIFY(action->isEnabled());
        if (action->objectName() == "NEW_RECORD_ACTION") {
            newAction = action;
            continue;
        }
        if (action->objectName() == "EDIT_RECORD_ACTION") {
            editAction = action;
            continue;
        }
        if (action->objectName() == "DELETE_RECORD_ACTION") {
            deleteAction = action;
            continue;
        }
        if (action->objectName() == "REFRESH_ACTION") {
            refreshAction = action;
            continue;
        }
        if (action->objectName() == "REPORT_ACTION") {
            reportAction = action;
            continue;
        }
    }
    QVERIFY(newAction != nullptr);
    QVERIFY(editAction != nullptr);
    QVERIFY(refreshAction != nullptr);
    QVERIFY(deleteAction != nullptr);
    QVERIFY(reportAction != nullptr);

    bool deleteMessageBoxShown = false;
    auto deleteMessageCallback = [&deleteMessageBoxShown]() {
        QMessageBox *msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::mouseClick(msgBox->button(QMessageBox::Yes), Qt::LeftButton);
        deleteMessageBoxShown = true;
    };
    QTimer::singleShot(200, deleteMessageCallback);
    deleteAction->trigger();
    QApplication::processEvents();
    QVERIFY(deleteMessageBoxShown);

    t.setId(0);
    manager.saveTeacher(t, error);
    refreshAction->trigger();
    QApplication::processEvents();
    tableView->selectRow(0);
    QApplication::processEvents();
    editAction->trigger();
    QApplication::processEvents();
    auto firstNameEdit =
        dynamic_cast<QLineEdit *>(editor->fieldEditors()["first_name"]);
    auto lastNameEdit =
        dynamic_cast<QLineEdit *>(editor->fieldEditors()["last_name"]);
    auto emailEdit = dynamic_cast<QLineEdit *>(editor->fieldEditors()["email"]);
    auto rfidEdit = dynamic_cast<QLineEdit *>(editor->fieldEditors()["rfid"]);
    auto employeeNumberEdit =
        dynamic_cast<QLineEdit *>(editor->fieldEditors()["employee_number"]);
    auto officeEdit =
        dynamic_cast<QLineEdit *>(editor->fieldEditors()["office"]);
    auto saveButton = dynamic_cast<QDialogButtonBox *>(
                          editor->findChild<QDialogButtonBox *>())
                          ->button(QDialogButtonBox::Save);
    lastNameEdit->setText("Janković");
    saveButton->click();
    QApplication::processEvents();
    tableView->selectRow(0);
    QApplication::processEvents();
    QCOMPARE(lastNameEdit->text(), QString("Janković"));

    auto oldRowCount = tableView->model()->rowCount();
    newAction->trigger();
    QApplication::processEvents();
    firstNameEdit->setText("Petar");
    lastNameEdit->setText("Petrovic");
    emailEdit->setText("pera@petar.com");
    rfidEdit->setText("RRFFIIDD");
    employeeNumberEdit->setText("EMP#123");
    officeEdit->setText("3");
    saveButton->click();
    QApplication::processEvents();
    QCOMPARE(tableView->model()->rowCount(), oldRowCount + 1);

    bool entryReportDialogShown = false;
    auto entryReportDialogCallback = [&entryReportDialogShown]() {
        auto reportDialog = dynamic_cast<EntryReportDialog *>(
            QApplication::activeModalWidget());
        QTest::keyClick(reportDialog, Qt::Key_Escape);
        entryReportDialogShown = true;
    };

    QTimer::singleShot(200, entryReportDialogCallback);
    reportAction->trigger();
    QApplication::processEvents();
    QVERIFY(entryReportDialogShown);
}
