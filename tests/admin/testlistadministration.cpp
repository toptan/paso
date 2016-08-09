#include "testlistadministration.h"

#include "list.h"
#include "listeditorwidget.h"
#include "listform.h"
#include "listtablemodel.h"
#include "listvalidator.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QSqlError>
#include <QSqlField>
#include <QTableView>

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
    QCOMPARE(result->text,
             QString("List expiry date cannot be in the past."));

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

void TestListAdministration::testListTableModel() {
    QVariantMap columnLabels{{"name", "Name"},
                             {"permanent", "Permanent"},
                             {"system", "System"},
                             {"expiry_date", "Expiry Date"}};
    ListTableModel model(columnLabels, QSqlDatabase::database(dbName));
    QCOMPARE(model.columnCount(), 6);
    QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("id"));
    QCOMPARE(model.headerData(1, Qt::Horizontal).toString(), QString("Name"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(),
             QString("id_course"));
    QCOMPARE(model.headerData(3, Qt::Horizontal).toString(), QString("System"));
    QCOMPARE(model.headerData(4, Qt::Horizontal).toString(),
             QString("Permanent"));
    QCOMPARE(model.headerData(5, Qt::Horizontal).toString(),
             QString("Expiry Date"));
}

void TestListAdministration::testListForm() {
    ListForm form;
    form.show();
    QTest::qWaitForWindowExposed(&form);
    auto tableView = form.findChild<QTableView *>();
    auto listEditorWidget = form.findChild<ListEditorWidget *>();

    QVERIFY(tableView != nullptr);
    QVERIFY(listEditorWidget);
}
