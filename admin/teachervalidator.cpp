#include "teachervalidator.h"

#include <QLineEdit>
#include <QSqlError>

using namespace std;
using namespace paso::widget;

namespace paso {
namespace admin {

TeacherValidator::TeacherValidator(const FieldTypes &fieldTypes,
                                   const FieldEditors &fieldEditors,
                                   QObject *parent)
    : PersonValidator(fieldTypes, fieldEditors, parent) {}

TeacherValidator::~TeacherValidator() {}

shared_ptr<ValidationError>
TeacherValidator::validate(const QSqlRecord &original) const {
    auto retVal = PersonValidator::validate(original);

    if (retVal) {
        return retVal;
    }

    retVal =
        validateEmployeeNumber(original.value("employee_number").toString());
    if (retVal) {
        return retVal;
    }

    return validateOfficeNumber();
}

std::shared_ptr<ValidationError>
TeacherValidator::validateEmployeeNumber(const QString &original) const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["employee_number"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The employee number cannot be left empty."));
    }
    if (original.isEmpty() || text != original) {
        QSqlError error;
        const auto unique = dbManager().employeeNumberUnique(text, error);
        if (error.type() != QSqlError::NoError) {
            return make_shared<ValidationError>(
                editor, tr("Critical error"),
                tr("There was an error working with the database."),
                error.text(), QMessageBox::Critical);
        }

        if (!unique) {
            return make_shared<ValidationError>(
                editor, tr("Invalid data entered"),
                QString(
                    tr("The teacher with employee number %1 already exists in "
                       "the system."))
                    .arg(text));
        }
    }
    return nullptr;
}

shared_ptr<ValidationError> TeacherValidator::validateOfficeNumber() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["office"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The office number cannot be left empty."));
    }

    return nullptr;
}
}
}
