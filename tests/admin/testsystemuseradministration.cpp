#include "testsystemuseradministration.h"

#include "pasodb.h"
#include "systemuser.h"
#include "systemusereditorwidget.h"
#include "systemusersform.h"
#include "systemusertablemodel.h"
#include "systemuservalidator.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlError>
#include <QSqlField>
#include <QTableView>
#include <QTimer>

using namespace paso::db;
using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::widget;
using namespace paso::admin;

TestSystemUserAdministration::TestSystemUserAdministration() : TestBase() {}

void TestSystemUserAdministration::testSystemUserValidator() {
    const FieldTypes fieldTypes{{"username", FieldType::LineEdit},
                                {"password", FieldType::PasswordEdit},
                                {"first_name", FieldType::LineEdit},
                                {"last_name", FieldType::LineEdit},
                                {"email", FieldType::LineEdit},
                                {"role", FieldType::ComboBox}};
    const QString title = "Invalid data entered";
    QSqlRecord emptyRecord;
    QSqlRecord nonEmptyRecord;
    nonEmptyRecord.append(QSqlField("id", QVariant::String));
    nonEmptyRecord.append(QSqlField("username", QVariant::String));
    nonEmptyRecord.append(QSqlField("password", QVariant::String));
    nonEmptyRecord.append(QSqlField("first_name", QVariant::String));
    nonEmptyRecord.append(QSqlField("last_name", QVariant::String));
    nonEmptyRecord.append(QSqlField("email", QVariant::String));
    nonEmptyRecord.append(QSqlField("role", QVariant::String));

    auto usernameEdit = new QLineEdit();
    auto passwordEdit = new QLineEdit();
    auto firstNameEdit = new QLineEdit();
    auto lastNameEdit = new QLineEdit();
    auto emailEdit = new QLineEdit();
    auto roleCombo = new QComboBox();
    for (const auto &role : enumeratedRoles.keys()) {
        roleCombo->addItem(roleToString(role), roleToString(role));
    }

    const FieldEditors fieldEditors{
        {"username", usernameEdit},    {"password", passwordEdit},
        {"first_name", firstNameEdit}, {"last_name", lastNameEdit},
        {"email", emailEdit},          {"role", roleCombo}};

    SystemUserValidator validator(fieldTypes, fieldEditors, this);

    auto result = validator.validate(emptyRecord);
    QVERIFY((bool)result);
    QCOMPARE(result->editor, usernameEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The username has to be at least four characters long and "
                     "cannot be longer than sixteen."));

    usernameEdit->setText("12345678901234567");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, usernameEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The username has to be at least four characters long and "
                     "cannot be longer than sixteen."));

    usernameEdit->setText("admin");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, usernameEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(
        result->text,
        QString("A user with entered username already exists in the system."));

    usernameEdit->setText("username");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, passwordEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The password has to be at least eight characters long "
                     "and cannot be longer than sixteen."));

    passwordEdit->setText("the_password");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, firstNameEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("You need to specify user's first name."));

    firstNameEdit->setText("PetarSaVrloDugačkimINeobičnimNemogućimImenom");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, firstNameEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The first name cannot be longer than 32 characters."));

    firstNameEdit->setText("Petar");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, lastNameEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text, QString("You need to specify user's last name."));

    lastNameEdit->setText(
        "PetrovićSaVrloDugačkimINeobičnimNemogućimPrezimenom");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, lastNameEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The last name cannot be longer than 32 characters."));

    lastNameEdit->setText("Petrović");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, emailEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(
        result->text,
        QString("You need to provide a valid email address for the user."));

    emailEdit->setText("petar<AT>pera.com");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, emailEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(
        result->text,
        QString("You need to provide a valid email address for the user."));

    emailEdit->setText("petar@petrovic.petrovic.petrovic.petrovic.petrovic."
                       "petrovic.petrovic.petrovic.com");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, emailEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The email cannot be longer than 64 characters."));

    emailEdit->setText("petar@pera.com");
    result = validator.validate(emptyRecord);
    QVERIFY(!(bool)result);

    nonEmptyRecord.setValue("username", "administrator");
    auto db = QSqlDatabase::database(dbName);
    db.exec("DROP TABLE SYSTEM_USER");
    result = validator.validate(nonEmptyRecord);
    QCOMPARE(result->editor, usernameEdit);
    QCOMPARE(result->title, QString("Critical error"));
    QCOMPARE(result->text,
             QString("There was an error working with the database."));
    QVERIFY(!result->detailedText.isEmpty());

    for (auto editor : fieldEditors.values()) {
        delete editor;
    }
}

void TestSystemUserAdministration::testSystemUserEditorWidget() {
    const FieldTypes fieldTypes = {{"username", FieldType::LineEdit},
                                   {"password", FieldType::PasswordEdit},
                                   {"first_name", FieldType::LineEdit},
                                   {"last_name", FieldType::LineEdit},
                                   {"email", FieldType::LineEdit},
                                   {"role", FieldType::ComboBox}};
    const QVariantMap columnLabels{
        {"username", "Username"},     {"password", "Password"},
        {"first_name", "First Name"}, {"last_name", "Last Name"},
        {"email", "Email"},           {"role", "Role"}};

    QSqlRecord record;
    record.append(QSqlField("id", QVariant::String));
    record.append(QSqlField("username", QVariant::String));
    record.append(QSqlField("password", QVariant::String));
    record.append(QSqlField("first_name", QVariant::String));
    record.append(QSqlField("last_name", QVariant::String));
    record.append(QSqlField("email", QVariant::String));
    record.append(QSqlField("role", QVariant::String));
    SystemUserEditorWidget editor(fieldTypes);
    editor.setupUi(columnLabels, record);
    editor.show();
    QTest::qWaitForWindowExposed(&editor);

    editor.onEditNewRecord(record);
    QApplication::processEvents();
    auto usernameEdit =
        dynamic_cast<QLineEdit *>(editor.fieldEditors()["username"]);
    auto passwordEdit =
        dynamic_cast<QLineEdit *>(editor.fieldEditors()["password"]);
    auto firstNameEdit =
        dynamic_cast<QLineEdit *>(editor.fieldEditors()["first_name"]);
    auto lastNameEdit =
        dynamic_cast<QLineEdit *>(editor.fieldEditors()["last_name"]);
    auto emailEdit = dynamic_cast<QLineEdit *>(editor.fieldEditors()["email"]);
    auto roleCombo = dynamic_cast<QComboBox *>(editor.fieldEditors()["role"]);
    QVERIFY(!usernameEdit->isReadOnly());
    QCOMPARE(usernameEdit->maxLength(), 16);
    QCOMPARE(passwordEdit->maxLength(), 32);
    QCOMPARE(firstNameEdit->maxLength(), 32);
    QCOMPARE(lastNameEdit->maxLength(), 32);
    QCOMPARE(emailEdit->maxLength(), 64);
    QCOMPARE(roleCombo->count(), enumeratedRoles.size());
    record.setValue("username", "root");
    editor.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(usernameEdit->isReadOnly());
}

void TestSystemUserAdministration::testSystemUserTableModel() {
    const QVariantMap columnLabels{
        {"username", "Username"},     {"password", "Password"},
        {"first_name", "First Name"}, {"last_name", "Last Name"},
        {"email", "Email"},           {"role", "Role"}};
    SystemUserTableModel model(columnLabels, QSqlDatabase::database(dbName));
    QCOMPARE(model.columnCount(), 7);
    QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("id"));
    QCOMPARE(model.headerData(1, Qt::Horizontal).toString(),
             QString("Username"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(),
             QString("Password"));
    QCOMPARE(model.headerData(3, Qt::Horizontal).toString(),
             QString("First Name"));
    QCOMPARE(model.headerData(4, Qt::Horizontal).toString(),
             QString("Last Name"));
    QCOMPARE(model.headerData(5, Qt::Horizontal).toString(), QString("Email"));
    QCOMPARE(model.headerData(6, Qt::Horizontal).toString(), QString("Role"));

    // Password should be hidden.
    auto index = model.index(0, 2);
    QCOMPARE(model.data(index).toString(), QString("<hidden>"));

    // Role string must be one in stringEnumbertedRoles
    index = model.index(0, 6);
    auto role = model.data(index).toString();
    bool found = false;
    for (auto temp : stringEnumeratedRoles) {
        if (role == temp) {
            found = true;
            break;
        }
    }
    QVERIFY(found);

    index = model.index(0, 0);
    QVERIFY(model.data(index) != QVariant());
}

void TestSystemUserAdministration::testSystemUsersForm() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DELETE FROM SYSTEM_USER");
    DBManager manager(dbName);

    SystemUsersForm form;
    form.show();
    QTest::qWaitForWindowExposed(&form);
    auto tableView = form.findChild<QTableView *>();
    auto editor = form.findChild<SystemUserEditorWidget *>();

    QVERIFY(tableView != nullptr);
    QVERIFY(editor != nullptr);

    tableView->selectRow(0);
    QApplication::processEvents();
    QAction *newAction = nullptr;
    QAction *editAction = nullptr;
    QAction *deleteAction = nullptr;
    QAction *refreshAction = nullptr;
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
    }
    QVERIFY(newAction != nullptr);
    QVERIFY(editAction != nullptr);
    QVERIFY(deleteAction != nullptr);
    QVERIFY(refreshAction != nullptr);
    QVERIFY(newAction->isEnabled());
    QVERIFY(editAction->isEnabled());
    QVERIFY(!deleteAction->isEnabled());
    QVERIFY(refreshAction->isEnabled());

    editAction->trigger();
    QApplication::processEvents();

    auto usernameEdit =
        dynamic_cast<QLineEdit *>(editor->fieldEditors()["username"]);
    auto passwordEdit =
        dynamic_cast<QLineEdit *>(editor->fieldEditors()["password"]);
    auto firstNameEdit =
        dynamic_cast<QLineEdit *>(editor->fieldEditors()["first_name"]);
    auto lastNameEdit =
        dynamic_cast<QLineEdit *>(editor->fieldEditors()["last_name"]);
    auto emailEdit = dynamic_cast<QLineEdit *>(editor->fieldEditors()["email"]);
    auto roleCombo = dynamic_cast<QComboBox *>(editor->fieldEditors()["role"]);
    auto saveButton = dynamic_cast<QDialogButtonBox *>(
                          editor->findChild<QDialogButtonBox *>())
                          ->button(QDialogButtonBox::Save);
    int newIndex = roleCombo->currentIndex() + 1;
    if (newIndex == roleCombo->count()) {
        newIndex = 0;
    }
    roleCombo->setCurrentIndex(newIndex);
    saveButton->click();
    QApplication::processEvents();
    QSqlError error;
    auto user = manager.getSystemUser("root", error);
    QCOMPARE(user->role(), SystemRole::SUPER_USER);

    db.exec("DELETE FROM SYSTEM_USER");
    newAction->trigger();
    QApplication::processEvents();
    usernameEdit->setText("toptan");
    passwordEdit->setText("toptan_password");
    firstNameEdit->setText("Toplica");
    lastNameEdit->setText("Tanasković");
    emailEdit->setText("toptan@paso.com");
    roleCombo->setCurrentIndex(0);
    saveButton->click();
    QApplication::processEvents();
    refreshAction->trigger();
    QApplication::processEvents();
    tableView->selectRow(0);
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
}
