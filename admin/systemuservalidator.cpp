#include "systemuservalidator.h"

#include <QLineEdit>
#include <QVariant>
#include <QStyle>
#include <QSqlError>
#include <QMessageBox>
#include <QRegExp>
#include <QRegExpValidator>

namespace paso {
namespace admin {

using namespace std;

SystemUserValidator::SystemUserValidator(const FieldTypes &fieldTypes,
                                         const FieldEditors &fieldEditors,
                                         QObject *parent)
    : RecordValidator(fieldTypes, fieldEditors, parent), dbManager() {}

shared_ptr<ValidationError>
SystemUserValidator::validate(const QSqlRecord &original) const {
    auto retVal = validateUsername(original.value("username").toString());

    if (retVal) {
        return retVal;
    }
    retVal = validatePassword();
    if (retVal) {
        return retVal;
    }
    retVal = validateFirstName();
    if (retVal) {
        return retVal;
    }
    retVal = validateLastName();
    if (retVal) {
        return retVal;
    }
    return validateEmail();
}

shared_ptr<ValidationError>
SystemUserValidator::validateUsername(const QString &original) const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["username"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty() || text.size() < 4 || text.size() > 16) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The username has to be at least four characters "
               "long and cannot be longer than sixteen."));
    }
    if (original.isEmpty() || text != original) {
        // Check if entered username is unique.
        QSqlError error;
        auto unique = dbManager.usernameUnique(text, error);
        if (error.type() != QSqlError::NoError) {
            return make_shared<ValidationError>(
                nullptr, tr("Critical error"),
                tr("There was an error working with the database."),
                error.text(), QMessageBox::Critical);
        }
        if (!unique) {
            return make_shared<ValidationError>(
                editor, tr("Invalid data entered"),
                tr("A user with entered username already exists in the "
                   "system."));
        }
    }
    return nullptr;
}

shared_ptr<ValidationError> SystemUserValidator::validatePassword() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["password"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty() || text.size() < 8 || text.size() > 16) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The password has to be at least eight characters "
               "long and cannot be longer than sixteen."));
    }
    return nullptr;
}

shared_ptr<ValidationError> SystemUserValidator::validateFirstName() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["first_name"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("You need to specify user's first name."));
    }
    if (text.size() > 32) {
        make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The first name cannot be longer than 32 characters."));
    }
    return nullptr;
}

shared_ptr<ValidationError> SystemUserValidator::validateLastName() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["last_name"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("You need to specify user's last name."));
    }
    if (text.size() > 32) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The first name cannot be longer than 32 characters."));
    }
    return nullptr;
}

shared_ptr<ValidationError> SystemUserValidator::validateEmail() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["email"]);
    auto text = editor->text().trimmed();
    QRegExp regExp("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b",
                   Qt::CaseInsensitive);
    QRegExpValidator validator(regExp);
    int pos = 0;
    if (validator.validate(text, pos) != QValidator::Acceptable) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("You need to provide a valid email address for the user."));
    }
    return nullptr;
}
}
}
