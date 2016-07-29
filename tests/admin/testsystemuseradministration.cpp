#include "testsystemuseradministration.h"

#include "systemuser.h"
#include "systemusereditorwidget.h"
#include "systemusertablemodel.h"
#include "systemuservalidator.h"

#include <QComboBox>
#include <QLineEdit>
#include <QSqlError>
#include <QSqlField>

using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::widget;
using namespace paso::admin;

TestSystemUserAdministration::TestSystemUserAdministration() : TestBase() {}

void TestSystemUserAdministration::testSystemUserValidator() {
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
        {"email_edit", emailEdit},     {"role", roleCombo}};

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

    usernameEdit->setText("username");
    result = validator.validate(emptyRecord);
    QCOMPARE(result->editor, passwordEdit);
    QCOMPARE(result->title, title);
    QCOMPARE(result->text,
             QString("The password has to be at least eight characters long "
                     "and cannot be longer than sixteen."));

    for (auto editor : fieldEditors.values()) {
        delete editor;
    }
}
