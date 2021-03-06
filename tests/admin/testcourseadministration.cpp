#include "testcourseadministration.h"

#include "course.h"
#include "coursedetailsdialog.h"
#include "courseeditorwidget.h"
#include "courseform.h"
#include "coursetablemodel.h"
#include "coursevalidator.h"
#include "logdialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QPoint>
#include <QPushButton>
#include <QSqlError>
#include <QSqlField>
#include <QTableView>
#include <QTimer>

using namespace paso::data::entity;
using namespace paso::widget;
using namespace paso::admin;

TestCourseAdministration::TestCourseAdministration() : TestBase() {}

void TestCourseAdministration::testCourseValidator() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("INSERT INTO COURSE(CODE, NAME) VALUES('XXXXX', 'YYYYY')");

    const FieldTypes fieldTypes{{"code", FieldType::LineEdit},
                                {"name", FieldType::LineEdit}};
    auto codeLineEdit = new QLineEdit();
    auto nameLineEdit = new QLineEdit();
    const FieldEditors fieldEditors{{"code", codeLineEdit},
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

    db.exec("DROP TABLE COURSE CASCADE");
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
    QVariantMap columnLabels{{"code", "Code"}, {"name", "Course"}};
    CourseTableModel model(columnLabels, QSqlDatabase::database(dbName));
    QCOMPARE(model.columnCount(), 3);
    QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("id"));
    QCOMPARE(model.headerData(1, Qt::Horizontal).toString(), QString("Code"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(), QString("Course"));
}

void TestCourseAdministration::testCourseForm() {
    CourseForm form;
    form.show();
    QTest::qWaitForWindowExposed(&form);
    auto tableView = form.findChild<QTableView *>();
    auto courseEditorWidget = form.findChild<CourseEditorWidget *>();

    QVERIFY(tableView != nullptr);
    QVERIFY(courseEditorWidget != nullptr);

    tableView->selectRow(0);
    QApplication::processEvents();
    QAction *newAction = nullptr;
    QAction *deleteAction = nullptr;
    QAction *importAction = nullptr;
    QAction *detailsAction = nullptr;
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
    QVERIFY(deleteAction != nullptr);
    QVERIFY(importAction != nullptr);
    QVERIFY(detailsAction != nullptr);

    bool deleteMessageBoxShown = false;
    auto deleteMessageCallback = [&deleteMessageBoxShown]() {
        QMessageBox *msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::keyClick(msgBox, Qt::Key_Escape);
        deleteMessageBoxShown = true;
    };
    QTimer::singleShot(200, deleteMessageCallback);
    deleteAction->trigger();
    QApplication::processEvents();
    QVERIFY(deleteMessageBoxShown);

    auto oldRowCount = tableView->model()->rowCount();
    newAction->trigger();
    QApplication::processEvents();
    auto codeEdit =
        dynamic_cast<QLineEdit *>(courseEditorWidget->fieldEditors()["code"]);
    auto nameEdit =
        dynamic_cast<QLineEdit *>(courseEditorWidget->fieldEditors()["name"]);
    auto saveButton = dynamic_cast<QDialogButtonBox *>(
                          courseEditorWidget->findChild<QDialogButtonBox *>())
                          ->button(QDialogButtonBox::Save);
    codeEdit->setText("XXXXX");
    nameEdit->setText("YYYYY");
    saveButton->click();
    QApplication::processEvents();
    QCOMPARE(tableView->model()->rowCount(), oldRowCount + 1);

    bool detailsDialogShown = false;
    auto detailsDialogCallback = [&detailsDialogShown]() {
        auto detailsDialog = dynamic_cast<CourseDetailsDialog *>(
            QApplication::activeModalWidget());
        QTest::keyClick(detailsDialog, Qt::Key_Escape);
        detailsDialogShown = true;
    };

    QTimer::singleShot(200, detailsDialogCallback);
    detailsAction->trigger();
    QApplication::processEvents();
    QVERIFY(detailsDialogShown);
    detailsDialogShown = false;
    QTimer::singleShot(200, detailsDialogCallback);
    QPoint point(tableView->columnViewportPosition(0) + 5,
                 tableView->rowViewportPosition(0) + 10);
    auto pViewport = tableView->viewport();

    QTest::mouseClick(pViewport, Qt::LeftButton, 0, point);
    QTest::mouseDClick(pViewport, Qt::LeftButton, 0, point);
    QApplication::processEvents();
    QVERIFY(detailsDialogShown);
}

void TestCourseAdministration::testCourseFormImportCourses() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DELETE FROM COURSE");
    CourseForm form;
    bool importDone = false;
    auto importDoneCallback = [&importDone]() { importDone = true; };
    connect(&form, &CourseForm::importDone, importDoneCallback);
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
        QTest::qWait(100);
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
                              "/files/courses_with_errors.csv");
    attempt = 0;
    while ((logDialog = dynamic_cast<LogDialog *>(
                QApplication::activeModalWidget())) == nullptr &&
           attempt < 10) {
        QTest::qWait(100);
        attempt++;
    }
    QVERIFY(logDialog != nullptr);
    while (!importDone) {
        QTest::qWait(100);
    }
    auto buttonBox = logDialog->findChild<QDialogButtonBox *>();
    buttonBox->button(QDialogButtonBox::Close)->click();
    delete logDialog;
    QApplication::processEvents();
    QCOMPARE(form.model()->rowCount(), 1);

    db.exec("DELETE FROM COURSE");
    logDialog = nullptr;
    importDone = false;
    form.onImportFileSelected(QDir::currentPath() + "/files/ispiti.csv");
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
    delete logDialog;
    QApplication::processEvents();
    QCOMPARE(form.model()->rowCount(), 77);

    db.exec("DELETE FROM COURSE");
    refreshAction->trigger();
    QApplication::processEvents();
    db.exec("DROP TABLE COURSE CASCADE");
    logDialog = nullptr;
    importDone = false;
    attempt = 0;
    form.onImportFileSelected(QDir::currentPath() +
                              "/files/courses_with_errors.csv");
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
    delete logDialog;
    QApplication::processEvents();
    QCOMPARE(form.model()->rowCount(), 0);
}
