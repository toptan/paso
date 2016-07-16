#include "personvalidator.h"

#include <QLineEdit>
#include <QVariant>
#include <QRegExpValidator>
#include <QSqlError>

namespace paso {
namespace admin {

using namespace std;

PersonValidator::PersonValidator(const FieldTypes &fieldTypes,
                                 const FieldEditors &fieldEditors,
                                 QObject *parent)
    : RecordValidator(fieldTypes, fieldEditors, parent) {}

shared_ptr<ValidationError>
PersonValidator::validate(const QSqlRecord &original) const {
    auto retVal = validateFirstName();

    if (retVal) {
        return retVal;
    }

    retVal = validateLastName();
    if (retVal) {
        return retVal;
    }

    return validateEmail();
}

shared_ptr<ValidationError> PersonValidator::validateFirstName() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["first_name"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("You need to specifiy person's first name."));
    }
    if (text.size() > 32) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The first name cannot be longer than 32 characters."));
    }

    return nullptr;
}

shared_ptr<ValidationError> PersonValidator::validateLastName() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["last_name"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("You need to specifiy person's last name."));
    }
    if (text.size() > 32) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The last name cannot be longer than 32 characters."));
    }

    return nullptr;
}

shared_ptr<ValidationError> PersonValidator::validateEmail() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["email"]);
    auto text = editor->text().trimmed();
    QRegExp regExp("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b",
                   Qt::CaseInsensitive);
    QRegExpValidator validator(regExp);
    int pos = 0;
    if (validator.validate(text, pos) != QValidator::Acceptable) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("You need to provide a valid person's email address."));
    }
    return nullptr;
}
}
}
