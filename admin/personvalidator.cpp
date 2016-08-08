#include "personvalidator.h"

#include <QLineEdit>
#include <QRegExpValidator>
#include <QSqlError>
#include <QVariant>

using namespace paso::widget;
using namespace std;

namespace paso {
namespace admin {

PersonValidator::PersonValidator(const FieldTypes &fieldTypes,
                                 const FieldEditors &fieldEditors,
                                 QObject *parent)
    : RecordValidator(fieldTypes, fieldEditors, parent) {}

PersonValidator::~PersonValidator() {}

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
            tr("You need to specify person's first name."));
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
            tr("You need to specify person's last name."));
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
    if (text.isEmpty()) {
        return nullptr;
    }

    if (text.size() > 64) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The email cannot be longer than 64 characters."));
    }

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
