#include "teststudentadministration.h"

#include "logdialog.h"
#include "pasodb.h"
#include "person.h"
#include "personvalidator.h"
#include "student.h"
#include "studenteditorwidget.h"
#include "studentform.h"
#include "studentquerymodel.h"
#include "studentvalidator.h"

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

TestStudentAdministration::TestStudentAdministration() : TestBase() {}

void TestStudentAdministration::testPersonValidator() {
    const FieldTypes fieldTypes{{"first_name", FieldType::LineEdit},
                                {"last_name", FieldType::LineEdit},
                                {"email", FieldType::LineEdit},
                                {"rfid", FieldType::LineEdit}};
    const QString title = "Invalid data entered";
    QSqlRecord emptyRecord;

    auto firstNameEdit = new QLineEdit();
    auto lastNameEdit = new QLineEdit();
    auto emailEdit = new QLineEdit();
    auto rfidEdit = new QLineEdit();
    const FieldEditors fieldEditors{{"first_name", firstNameEdit},
                                    {"last_name", lastNameEdit},
                                    {"email", emailEdit},
                                    {"rfid", rfidEdit}};

    PersonValidator validator(fieldTypes, fieldEditors, this);

    auto result = validator.validate(emptyRecord);
    QVERIFY((bool)result);
    QCOMPARE(result->editor, firstNameEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("You need to specify person's first name."));

    firstNameEdit->setText("123456789012345678901234567890123456");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, firstNameEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The first name cannot be longer than 32 characters."));

    firstNameEdit->setText("Toplica");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, lastNameEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("You need to specify person's last name."));

    lastNameEdit->setText("123456789012345678901234567890123456");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, lastNameEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The last name cannot be longer than 32 characters."));

    lastNameEdit->setText("Tanasković");
    result = validator.validate(emptyRecord);
    QVERIFY(!(bool)result);

    emailEdit->setText("toplica<AT>toplica.com");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, emailEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("You need to provide a valid person's email address."));

    emailEdit->setText("toplica@"
                       "1234567890123456789012345678901234567890123456789012345"
                       "67890.com");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, emailEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The email cannot be longer than 64 characters."));

    emailEdit->setText("toplica@toplica.com");
    result = validator.validate(emptyRecord);
    QVERIFY(!(bool)result);
}

void TestStudentAdministration::testStudentValidator() {
    const FieldTypes fieldTypes{{"first_name", FieldType::LineEdit},
                                {"last_name", FieldType::LineEdit},
                                {"email", FieldType::LineEdit},
                                {"rfid", FieldType::LineEdit},
                                {"index_number", FieldType::MaskedLineEdit},
                                {"year_of_study", FieldType::NumberEdit}};
    const QString title = "Invalid data entered";
    QSqlRecord emptyRecord;

    auto firstNameEdit = new QLineEdit();
    auto lastNameEdit = new QLineEdit();
    auto emailEdit = new QLineEdit();
    auto rfidEdit = new QLineEdit();
    auto indexNumberEdit = new QLineEdit();
    auto yearOfStudyEdit = new QSpinBox();
    const FieldEditors fieldEditors{{"first_name", firstNameEdit},
                                    {"last_name", lastNameEdit},
                                    {"email", emailEdit},
                                    {"rfid", rfidEdit},
                                    {"index_number", indexNumberEdit},
                                    {"year_of_study", yearOfStudyEdit}};

    StudentValidator validator(fieldTypes, fieldEditors, this);
    firstNameEdit->setText("Toplica");
    lastNameEdit->setText("Tanasković");
    emailEdit->setText("toplica@tanaskovic.com");

    auto result = validator.validate(emptyRecord);
    QVERIFY((bool)result);
    QCOMPARE(result->editor, indexNumberEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The index number must be in format YYYY/NNNN."));

    indexNumberEdit->setText("999/99999");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, indexNumberEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The index number must be in format YYYY/NNNN."));

    indexNumberEdit->setText("2020/1234");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, indexNumberEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("Enrolment year must be between 1990 and %1.")
                 .arg(QDate::currentDate().year()));

    indexNumberEdit->setText("1989/1234");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, indexNumberEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("Enrolment year must be between 1990 and %1.")
                 .arg(QDate::currentDate().year()));

    indexNumberEdit->setText("2015/0000");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, indexNumberEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The index number has to be greater than zero."));

    indexNumberEdit->setText("2015/-145");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, indexNumberEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The index number has to be greater than zero."));

    DBManager manager(dbName);
    QSqlError error;
    manager.importStudent("2003/2003,Ivana, Ivanovic,,2", error);
    indexNumberEdit->setText("2003/2003");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, indexNumberEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("The student with index number 2003/2003 "
                                   "already exists in the system."));

    indexNumberEdit->setText("2004/2004");
    yearOfStudyEdit->setValue(-2);
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, yearOfStudyEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("Year of study must be between 1 and 7."));

    yearOfStudyEdit->setValue(9);
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, yearOfStudyEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("Year of study must be between 1 and 7."));

    yearOfStudyEdit->setValue(3);
    result = validator.validate(emptyRecord);
    QVERIFY(!(bool)result);

    firstNameEdit->clear();
    result = validator.validate(emptyRecord);
    QVERIFY((bool)result);

    auto db = QSqlDatabase::database(dbName);
    db.exec("DROP TABLE STUDENT");
    firstNameEdit->setText("Toplica");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, indexNumberEdit);
    QCOMPARE(result->title, QString("Critical error"));
    QCOMPARE(result->text,
             QString("There was an error working with the database."));
    QVERIFY(!result->detailedText.isEmpty());
}

void TestStudentAdministration::testStudentEditorWidget() {
    const FieldTypes fieldTypes{{"first_name", FieldType::LineEdit},
                                {"last_name", FieldType::LineEdit},
                                {"email", FieldType::LineEdit},
                                {"rfid", FieldType::LineEdit},
                                {"index_number", FieldType::MaskedLineEdit},
                                {"year_of_study", FieldType::NumberEdit}};

    const QVariantMap columnLabels = {
        {"first_name", QObject::tr("First Name")},
        {"last_name", QObject::tr("Last Name")},
        {"email", QObject::tr("Email")},
        {"rfid", QObject::tr("RFID")},
        {"index_number", QObject::tr("Index Number")},
        {"year_of_study", QObject::tr("Year of Study")}};

    QSqlRecord record;
    record.append(QSqlField("id", QVariant::String));
    record.append(QSqlField("first_name", QVariant::String));
    record.append(QSqlField("last_name", QVariant::String));
    record.append(QSqlField("email", QVariant::String));
    record.append(QSqlField("rfid", QVariant::String));
    record.append(QSqlField("index_number", QVariant::String));
    record.append(QSqlField("year_of_study", QVariant::String));

    StudentEditorWidget editor(fieldTypes);
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
    auto indexNumberEdit =
        dynamic_cast<QLineEdit *>(editor.fieldEditors()["index_number"]);
    auto yearOfStudyEdit =
        dynamic_cast<QSpinBox *>(editor.fieldEditors()["year_of_study"]);

    QVERIFY(!firstNameEdit->isReadOnly());
    QVERIFY(!lastNameEdit->isReadOnly());
    QVERIFY(!emailEdit->isReadOnly());
    QVERIFY(!rfidEdit->isReadOnly());
    QVERIFY(!indexNumberEdit->isReadOnly());
    QVERIFY(yearOfStudyEdit->isEnabled());

    QCOMPARE(firstNameEdit->maxLength(), 32);
    QCOMPARE(lastNameEdit->maxLength(), 32);
    QCOMPARE(emailEdit->maxLength(), 64);
    QCOMPARE(rfidEdit->maxLength(), 64);
    QCOMPARE(indexNumberEdit->maxLength(), 9);
    QCOMPARE(indexNumberEdit->inputMask(), QString("9999/9999"));
    QCOMPARE(yearOfStudyEdit->minimum(), 1);
    QCOMPARE(yearOfStudyEdit->maximum(), 7);
}

void TestStudentAdministration::testStudentQueryModel() {
    const QVariantMap columnLabels = {
        {"first_name", QObject::tr("First Name")},
        {"last_name", QObject::tr("Last Name")},
        {"email", QObject::tr("Email")},
        {"rfid", QObject::tr("RFID")},
        {"index_number", QObject::tr("Index Number")},
        {"year_of_study", QObject::tr("Year of Study")}};

    StudentQueryModel model(columnLabels, QSqlDatabase::database(dbName), this);
    QCOMPARE(model.columnCount(), 7);
    QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("id"));
    QCOMPARE(model.headerData(1, Qt::Horizontal).toString(),
             QString("Index Number"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(),
             QString("Last Name"));
    QCOMPARE(model.headerData(3, Qt::Horizontal).toString(),
             QString("First Name"));
    QCOMPARE(model.headerData(4, Qt::Horizontal).toString(), QString("Email"));
    QCOMPARE(model.headerData(5, Qt::Horizontal).toString(), QString("RFID"));
    QCOMPARE(model.headerData(6, Qt::Horizontal).toString(),
             QString("Year of Study"));
}

void TestStudentAdministration::testStudentForm() {
    DBManager manager(dbName);
    QSqlError error;
    manager.importStudent("1996/0164, Tanaskovic,Toplica,toplica@gmail.com,4",
                          error);

    StudentForm form;
    form.show();
    QTest::qWaitForWindowExposed(&form);
    auto tableView = form.findChild<QTableView *>();
    auto editor = form.findChild<StudentEditorWidget *>();

    QVERIFY(tableView != nullptr);
    QVERIFY(editor != nullptr);

    tableView->selectRow(0);
    QApplication::processEvents();
    QAction *newAction = nullptr;
    QAction *editAction = nullptr;
    QAction *deleteAction = nullptr;
    QAction *refreshAction = nullptr;
    QAction *importAction = nullptr;
    QAction *detailsAction = nullptr;
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
        if (action->objectName() == "IMPORT_ACTION") {
            importAction = action;
            continue;
        }
        if (action->objectName() == "DETAILS_ACTION") {
            detailsAction = action;
            continue;
        }
    }
    QVERIFY(newAction != nullptr);
    QVERIFY(editAction != nullptr);
    QVERIFY(refreshAction != nullptr);
    QVERIFY(deleteAction != nullptr);
    QVERIFY(importAction != nullptr);
    QVERIFY(detailsAction != nullptr);

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

    manager.importStudent("2002/2002, Jankovic,Janko,janko@gmail.com,4", error);
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
    auto indexNumberEdit =
        dynamic_cast<QLineEdit *>(editor->fieldEditors()["index_number"]);
    auto yearOfStudyEdit =
        dynamic_cast<QSpinBox *>(editor->fieldEditors()["year_of_study"]);
    auto saveButton = dynamic_cast<QDialogButtonBox *>(
                          editor->findChild<QDialogButtonBox *>())
                          ->button(QDialogButtonBox::Save);
    lastNameEdit->setText("Janković");
    saveButton->click();
    QApplication::processEvents();
    auto student = manager.getStudentByIndexNumber("2002/2002", error);
    QCOMPARE(student->lastName(), QString("Janković"));

    auto oldRowCount = tableView->model()->rowCount();
    newAction->trigger();
    QApplication::processEvents();
    firstNameEdit->setText("Petar");
    lastNameEdit->setText("Petrovic");
    emailEdit->setText("pera@petar.com");
    rfidEdit->setText("RRFFIIDD");
    indexNumberEdit->setText("2003/2003");
    yearOfStudyEdit->setValue(2);
    saveButton->click();
    QApplication::processEvents();
    QCOMPARE(tableView->model()->rowCount(), oldRowCount + 1);

    // TODO: Details tests.
}

void TestStudentAdministration::testStudentFormImportStudents() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DELETE FROM PERSON");
    StudentForm form;
    bool importDone = false;
    auto importDoneCallback = [&importDone]() { importDone = true; };
    connect(&form, &StudentForm::importDone, importDoneCallback);
    form.show();
    QTest::qWaitForWindowExposed(&form);

    QAction *importAction = nullptr;
    QAction *refreshAction = nullptr;
    for (auto action : form.toolBarActions()) {
        if (action->objectName() == "IMPORT_ACTION") {
            importAction = action;
            continue;
        }
        if (action->objectName() == "REFRESH_ACTION") {
            refreshAction = action;
            continue;
        }
    }

    importAction->trigger();
    QApplication::processEvents();
    QFileDialog *fileOpenDialog = nullptr;
    int attempt = 0;
    while ((fileOpenDialog = dynamic_cast<QFileDialog *>(
                QApplication::activeModalWidget())) == nullptr &&
           attempt < 10) {
        QTest::qWait(200);
        attempt++;
    }
    // Is file open dialog shown?
    QVERIFY(fileOpenDialog != nullptr);
    QTest::keyClick(fileOpenDialog, Qt::Key_Escape);
    QApplication::processEvents();
    auto importWithNonExistingFileCallback = [&form]() {
        form.onImportFileSelected("non_existing_file");
    };
    bool errorMessageBoxShown = false;
    auto errorMessageBoxShownCallback = [&errorMessageBoxShown]() {
        auto msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::keyClick(msgBox, Qt::Key_Return);
        errorMessageBoxShown = true;
    };

    QTimer::singleShot(0, importWithNonExistingFileCallback);
    QTimer::singleShot(0, errorMessageBoxShownCallback);
    QApplication::processEvents();
    QVERIFY(errorMessageBoxShown);
    QCOMPARE(form.model()->rowCount(), 0);

    LogDialog *logDialog = nullptr;
    form.onImportFileSelected(QDir::currentPath() +
                              "/files/students_with_errors.csv");
    attempt = 0;
    while ((logDialog = dynamic_cast<LogDialog *>(
                QApplication::activeModalWidget())) == nullptr &&
           attempt < 10) {
        QTest::qWait(200);
        attempt++;
    }
    QVERIFY(logDialog != nullptr);
    while (!importDone) {
        QTest::qWait(100);
    }
    auto buttonBox = logDialog->findChild<QDialogButtonBox *>();
    buttonBox->button(QDialogButtonBox::Close)->click();
    QApplication::processEvents();
    delete logDialog;
    QCOMPARE(form.model()->rowCount(), 1);

    db.exec("DELETE FROM PERSON");
    logDialog = nullptr;
    importDone = false;
    form.onImportFileSelected(QDir::currentPath() + "/files/students.csv");
    attempt = 0;
    while ((logDialog = dynamic_cast<LogDialog *>(
                QApplication::activeModalWidget())) == nullptr &&
           attempt < 10) {
        QTest::qWait(200);
        attempt++;
    }
    QVERIFY(logDialog != nullptr);
    while (!importDone) {
        QTest::qWait(100);
    }
    buttonBox = logDialog->findChild<QDialogButtonBox *>();
    buttonBox->button(QDialogButtonBox::Close)->click();
    QApplication::processEvents();
    delete logDialog;
    QCOMPARE(form.model()->rowCount(), 13);

    db.exec("DELETE FROM PERSON");
    refreshAction->trigger();
    QApplication::processEvents();
    db.exec("DROP TABLE STUDENT");
    logDialog = nullptr;
    importDone = false;
    attempt = 0;
    form.onImportFileSelected(QDir::currentPath() +
                              "/files/students_with_errors.csv");
    while ((logDialog = dynamic_cast<LogDialog *>(
                QApplication::activeModalWidget())) == nullptr &&
           attempt < 10) {
        QTest::qWait(200);
        attempt++;
    }
    QVERIFY(logDialog != nullptr);
    while (!importDone) {
        QTest::qWait(100);
    }
    buttonBox = logDialog->findChild<QDialogButtonBox *>();
    buttonBox->button(QDialogButtonBox::Close)->click();
    QApplication::processEvents();
    delete logDialog;
    QCOMPARE(form.model()->rowCount(), 0);
}
