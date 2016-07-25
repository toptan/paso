#include "coursevalidator.h"

#include <QLineEdit>
#include <QMessageBox>
#include <QSqlError>
#include <QVariant>

using namespace std;
using namespace paso::widget;

namespace paso {
namespace admin {

CourseValidator::CourseValidator(const FieldTypes &fieldTypes,
                                 const FieldEditors &fieldEditors,
                                 QObject *parent)
    : RecordValidator(fieldTypes, fieldEditors, parent) {}

CourseValidator::~CourseValidator() {}

shared_ptr<ValidationError>
CourseValidator::validate(const QSqlRecord &original) const {
    auto retVal = validateCode(original.value("code").toString());
    if (retVal) {
        return retVal;
    }

    return validateName();
}

shared_ptr<ValidationError>
CourseValidator::validateCode(const QString &original) const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["code"]);
    auto text = editor->text().trimmed();

    if (text.isEmpty()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The course code has to be provided."));
    }

    if (text.size() > 8) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The course code cannot exceed 8 characters."));
    }

    if (original.isEmpty() || text != original) {
        QSqlError error;
        const auto unique = dbManager().courseCodeUnique(text, error);
        if (error.type() != QSqlError::NoError) {
            return make_shared<ValidationError>(
                nullptr, tr("Critical error"),
                tr("There was an error working with the database."),
                error.text(), QMessageBox::Critical);
        }
        if (!unique) {
            return make_shared<ValidationError>(
                editor, tr("Invalid data entered"),
                tr("The course with entered code already exists in the "
                   "system."));
        }
    }
    return nullptr;
}

shared_ptr<ValidationError> CourseValidator::validateName() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["name"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The name of the course cannot be left emtpy."));
    }
    if (text.size() > 64) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The name of the course cannot exceed 64 characters."));
    }
    return nullptr;
}
}
}
