#include "testlistadministration.h"

#include "list.h"
#include "listdetailsdialog.h"
#include "listeditorwidget.h"
#include "listform.h"
#include "listtablemodel.h"
#include "listvalidator.h"

#include <QAction>
#include <QCheckBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlError>
#include <QSqlField>
#include <QTableView>
#include <QTimer>

#include <memory>

using namespace std;
using namespace paso::data::entity;
using namespace paso::widget;
using namespace paso::admin;

TestListAdministration::TestListAdministration() : TestBase() {}

void TestListAdministration::testListValidator() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("INSERT INTO LIST(NAME) VALUES('List name 1')");

    FieldTypes fieldTypes{{"name", FieldType::LineEdit},
                          {"system", FieldType::CheckBox},
                          {"permanent", FieldType::CheckBox},
                          {"expiry_date", FieldType::DateEdit}};
    auto nameLineEdit = new QLineEdit();
    auto systemCheckBox = new QCheckBox();
    auto permanentCheckBox = new QCheckBox();
    auto dateEdit = new QDateEdit();
    FieldEditors fieldEditors{{"name", nameLineEdit},
                              {"system", systemCheckBox},
                              {"permanent", permanentCheckBox},
                              {"expiry_date", dateEdit}};
    const QString title = "Invalid data entered";
    const QSqlRecord emptyRecord;
    QSqlRecord notEmptyRecord;
    notEmptyRecord.append(QSqlField("name", QVariant::String));
    notEmptyRecord.append(QSqlField("system", QVariant::Bool));
    notEmptyRecord.append(QSqlField("permanent", QVariant::Bool));
    notEmptyRecord.append(QSqlField("expiry_date", QVariant::Date));

    ListValidator validator(fieldTypes, fieldEditors, this);
    permanentCheckBox->setChecked(true);

    auto result = validator.validate(emptyRecord);
    QVERIFY((bool)result);
    QCOMPARE(result->editor, nameLineEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("The list name has to be provided."));

    nameLineEdit->setText("123456789012345678901234567890123456789012345678"
                          "90123456789012345");
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

    notEmptyRecord.setValue("expiry_date", QDate::currentDate().addDays(-1));
    result = validator.validate(notEmptyRecord);
    QVERIFY(!(bool)result);

    permanentCheckBox->setChecked(false);
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, dateEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("List expiry date cannot be in the past."));

    db.close();
    result = validator.validate(notEmptyRecord);
    QCOMPARE(result->editor, nameLineEdit);
    QCOMPARE(result->title, QString("Critical error"));
    QCOMPARE(result->text,
             QString("There was an error working with the database."));
    QCOMPARE(result->icon, QMessageBox::Critical);
    for (auto editor : fieldEditors.values()) {
        delete editor;
    }
}

void TestListAdministration::testListEditorWidget() {
    FieldTypes fieldTypes{{"name", FieldType::LineEdit},
                          {"system", FieldType::CheckBox},
                          {"permanent", FieldType::CheckBox},
                          {"expiry_date", FieldType::DateEdit}};
    QVariantMap columnLabels = {{"name", "Name"},
                                {"system", "System"},
                                {"permanent", "Permanent"},
                                {"expiry_date", "Expiry date"}};
    QSqlRecord record;
    record.append(QSqlField("id", QVariant::ULongLong));
    record.append(QSqlField("name", QVariant::String));
    record.append(QSqlField("permanent", QVariant::Bool));
    record.append(QSqlField("system", QVariant::Bool));
    record.append(QSqlField("expiry_date", QVariant::Date));
    ListEditorWidget editor(fieldTypes);
    editor.setupUi(columnLabels, record);
    editor.onEditNewRecord(record);
    QApplication::processEvents();
    auto nameEdit = dynamic_cast<QLineEdit *>(editor.fieldEditors()["name"]);
    auto permanentCheck =
        dynamic_cast<QCheckBox *>(editor.fieldEditors()["permanent"]);
    auto systemCheck =
        dynamic_cast<QCheckBox *>(editor.fieldEditors()["system"]);
    auto expiryEdit =
        dynamic_cast<QDateEdit *>(editor.fieldEditors()["expiry_date"]);

    QVERIFY(!nameEdit->isReadOnly());
    QCOMPARE(nameEdit->maxLength(), 64);
    QVERIFY(permanentCheck->isEnabled());
    QVERIFY(!systemCheck->isEnabled());
    QVERIFY(!expiryEdit->isReadOnly());
    QCOMPARE(expiryEdit->minimumDate(), QDate::currentDate());

    permanentCheck->setChecked(true);
    QApplication::processEvents();
    QVERIFY(expiryEdit->isReadOnly());

    permanentCheck->setChecked(false);
    QApplication::processEvents();
    QVERIFY(!expiryEdit->isReadOnly());
}

void TestListAdministration::testListTableModel() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DELETE FROM LIST");
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT, EXPIRY_DATE)"
            "          VALUES('L1', 0, 0, '1977-01-05')");
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT, EXPIRY_DATE)"
            "          VALUES('L2', 0, 1, '1978-01-05')");
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT, EXPIRY_DATE)"
            "          VALUES('L3', 1, 0, '1979-01-05')");
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT, EXPIRY_DATE)"
            "          VALUES('L4', 1, 1, '1980-01-05')");

    const QVariantMap columnLabels{{"name", "Name"},
                                   {"permanent", "Permanent"},
                                   {"system", "System"},
                                   {"expiry_date", "Expiry Date"}};
    ListTableModel model(columnLabels, db);
    QCOMPARE(model.columnCount(), 6);
    QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("id"));
    QCOMPARE(model.headerData(1, Qt::Horizontal).toString(), QString("Name"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(), QString("System"));
    QCOMPARE(model.headerData(3, Qt::Horizontal).toString(),
             QString("Permanent"));
    QCOMPARE(model.headerData(4, Qt::Horizontal).toString(),
             QString("Expiry Date"));
    QCOMPARE(model.headerData(5, Qt::Horizontal).toString(),
             QString("id_course"));

    auto index = model.index(0, 2);
    QCOMPARE(model.data(index).toString(), QString("No"));
    index = model.index(0, 3);
    QCOMPARE(model.data(index).toString(), QString("No"));
    index = model.index(1, 2);
    QCOMPARE(model.data(index).toString(), QString("No"));
    index = model.index(1, 3);
    QCOMPARE(model.data(index).toString(), QString("Yes"));
    index = model.index(2, 2);
    QCOMPARE(model.data(index).toString(), QString("Yes"));
    index = model.index(2, 3);
    QCOMPARE(model.data(index).toString(), QString("No"));
    index = model.index(3, 2);
    QCOMPARE(model.data(index).toString(), QString("Yes"));
    index = model.index(3, 3);
    QCOMPARE(model.data(index).toString(), QString("Yes"));
    index = model.index(0, 4);
    QCOMPARE(model.data(index).toString(), QString("05.01.1977."));
    index = model.index(1, 4);
    QVERIFY(model.data(index).isNull());
    index = model.index(2, 4);
    QVERIFY(model.data(index).isNull());
    index = model.index(3, 4);
    QVERIFY(model.data(index).isNull());
}

void TestListAdministration::testListForm() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DELETE FROM LIST");
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT, EXPIRY_DATE)"
            "          VALUES('L1', 0, 0, '1977-01-05')");
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT, EXPIRY_DATE)"
            "          VALUES('L2', 1, 0, '1977-01-05')");

    ListForm form;
    form.show();
    QTest::qWaitForWindowExposed(&form);
    auto tableView = form.findChild<QTableView *>();
    auto editor = form.findChild<ListEditorWidget *>();
    auto model = dynamic_cast<ListTableModel *>(form.model());
    auto name = dynamic_cast<QLineEdit *>(editor->fieldEditors()["name"]);
    auto permanent =
        dynamic_cast<QCheckBox *>(editor->fieldEditors()["permanent"]);
    auto expiryDate =
        dynamic_cast<QDateEdit *>(editor->fieldEditors()["expiry_date"]);
    auto buttonBox = editor->findChild<QDialogButtonBox *>();
    auto saveButton = buttonBox->button(QDialogButtonBox::Save);
    QAction *newAction = nullptr;
    QAction *editAction = nullptr;
    QAction *deleteAction = nullptr;
    QAction *refreshAction = nullptr;
    QAction *detailsAction = nullptr;
    for (auto action : form.toolBarActions()) {
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
        if (action->objectName() == "DETAILS_ACTION") {
            detailsAction = action;
            continue;
        }
    }
    QVERIFY(tableView != nullptr);
    QVERIFY(editor != nullptr);
    QCOMPARE(tableView->model()->rowCount(), 2);

    QVERIFY(newAction != nullptr);
    QVERIFY(editAction != nullptr);
    QVERIFY(deleteAction != nullptr);
    QVERIFY(refreshAction != nullptr);
    QVERIFY(detailsAction != nullptr);

    // No selection states
    QVERIFY(newAction->isEnabled());
    QVERIFY(!editAction->isEnabled());
    QVERIFY(!deleteAction->isEnabled());
    QVERIFY(refreshAction->isEnabled());
    QVERIFY(!detailsAction->isEnabled());

    tableView->selectRow(0);
    QApplication::processEvents();
    QVERIFY(newAction->isEnabled());
    QVERIFY(editAction->isEnabled());
    QVERIFY(deleteAction->isEnabled());
    QVERIFY(refreshAction->isEnabled());
    QVERIFY(detailsAction->isEnabled());

    // System lists cannot be modified.
    tableView->selectRow(1);
    QApplication::processEvents();
    QVERIFY(newAction->isEnabled());
    QVERIFY(!editAction->isEnabled());
    QVERIFY(!deleteAction->isEnabled());
    QVERIFY(refreshAction->isEnabled());
    QVERIFY(detailsAction->isEnabled());

    // Making list permanent and saving should make expiry date NULL.
    tableView->selectRow(0);
    QApplication::processEvents();
    editAction->trigger();
    QApplication::processEvents();
    permanent->setChecked(true);
    saveButton->click();
    QApplication::processEvents();
    QVERIFY(model->record(0).value("expiry_date").isNull());

    // Non permanent lists should have expiry date set.
    newAction->trigger();
    QApplication::processEvents();
    name->setText("L3");
    permanent->setChecked(false);
    expiryDate->setDate(QDate::currentDate().addMonths(1));
    saveButton->click();
    QApplication::processEvents();
    QCOMPARE(model->rowCount(), 3);
    QCOMPARE(model->record(2).value("expiry_date").toDate(),
             QDate::currentDate().addMonths(1));

    tableView->selectRow(2);
    QApplication::processEvents();
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

    bool detailsDialogShown = false;
    auto detailsDialogCallback = [&detailsDialogShown]() {
        auto detailsDialog = dynamic_cast<ListDetailsDialog *>(
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
