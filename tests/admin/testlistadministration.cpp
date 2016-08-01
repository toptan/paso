#include "testlistadministration.h"

#include "list.h"
#include "listeditorwidget.h"
#include "listtablemodel.h"
#include "listvalidator.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QSqlError>
#include <QSqlField>

using namespace paso::data::entity;
using namespace paso::widget;
using namespace paso::admin;

TestListAdministration::TestListAdministration() : TestBase() {}

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

    delete nameLineEdit;
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
