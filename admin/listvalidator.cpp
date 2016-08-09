#include "listvalidator.h"

#include <QCheckBox>
#include <QDateEdit>
#include <QDebug>
#include <QLineEdit>
#include <QSqlError>

using namespace std;

using namespace paso::widget;

namespace paso {
namespace admin {

ListValidator::ListValidator(const FieldTypes &fieldTypes,
                             const FieldEditors &fieldEditors, QObject *parent)
    : RecordValidator(fieldTypes, fieldEditors, parent) {}

ListValidator::~ListValidator() {}

shared_ptr<ValidationError>
ListValidator::validate(const QSqlRecord &original) const {
    auto system =
        dynamic_cast<QCheckBox *>(mFieldEditors["system"])->isChecked();
    auto permanent =
        dynamic_cast<QCheckBox *>(mFieldEditors["permanent"])->isChecked();
    auto retVal = validateName(original.value("name").toString());

    if (retVal) {
        return retVal;
    }

    if (permanent || system) {
        return nullptr;
    }

    return validateExpiryDate(original.value("expiry_date").toDate());
}

shared_ptr<ValidationError>
ListValidator::validateName(const QString &original) const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["name"]);
    auto text = editor->text().trimmed();

    if (text.isEmpty()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The list name has to be provided."));
    }

    if (text.size() > 64) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The list name cannot exceed 64 characters."));
    }

    if (original.isEmpty() || text != original) {
        QSqlError error;
        const auto unique = dbManager().listNameUnique(text, error);
        if (error.type() != QSqlError::NoError) {
            return make_shared<ValidationError>(
                editor, tr("Critical error"),
                tr("There was an error working with the database."),
                error.text(), QMessageBox::Critical);
        }
        if (!unique) {
            return make_shared<ValidationError>(
                editor, tr("Invalid data entered"),
                tr("The list with entered name already exists in the "
                   "system."));
        }
    }

    return nullptr;
}

shared_ptr<ValidationError>
ListValidator::validateExpiryDate(const QDate &original) const {
    auto editor = dynamic_cast<QDateEdit *>(mFieldEditors["expiry_date"]);
    auto date = editor->date();

    if (date < QDate::currentDate()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("List expiry date cannot be in the past."));
    }

    return nullptr;
}
}
}
