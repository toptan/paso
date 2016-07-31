#include "teststudentadministration.h"

#include "person.h"
#include "personvalidator.h"

#include <QLineEdit>
#include <QSqlField>

using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::widget;
using namespace paso::admin;

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

void TestStudentAdministration::testStudentValidator() {}

void TestStudentAdministration::testStudentEditorWidget() {}

void TestStudentAdministration::testStudentQueryModel() {}
