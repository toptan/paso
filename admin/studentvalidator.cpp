#include "studentvalidator.h"

#include <QDate>
#include <QLineEdit>
#include <QSpinBox>
#include <QSqlError>

using namespace std;
using namespace paso::widget;

namespace paso {
namespace admin {

StudentValidator::StudentValidator(const FieldTypes &fieldTypes,
                                   const FieldEditors &fieldEditors,
                                   QObject *parent)
    : PersonValidator(fieldTypes, fieldEditors, parent) {}

shared_ptr<ValidationError>
StudentValidator::validate(const QSqlRecord &original) const {
    auto retVal = PersonValidator::validate(original);

    if (retVal) {
        return retVal;
    }

    retVal = validateIndexNumber(original.value("index_number").toString());
    if (retVal) {
        return retVal;
    }

    return validateYearOfStudy();
}

shared_ptr<ValidationError>
StudentValidator::validateIndexNumber(const QString &original) const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["index_number"]);
    auto text = editor->text().trimmed();
    if (text.size() != 9 || text.count("/") != 1) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The index number must be in format YYYY/NNNN."));
    }
    auto split = text.split("/");
    if (split[0].size() != 4 || split[1].size() != 4) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The index number must be in format YYYY/NNNN."));
    }
    if (split[0].toInt() < 1990 ||
        split[0].toInt() > QDate::currentDate().year()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            QString(tr("Enrollment year must be between 1990 and %1."))
                .arg(QDate::currentDate().year()));
    }
    if (split[1].toInt() < 1) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The index number must be greater than zero."));
    }

    if (original.isEmpty() || text != original) {
        QSqlError error;
        const auto unique = dbManager().indexNumberUnique(text, error);
        if (error.type() != QSqlError::NoError) {
            return make_shared<ValidationError>(
                nullptr, tr("Critical error"),
                tr("There was an error working with the database."),
                error.text(), QMessageBox::Critical);
        }

        if (!unique) {
            return make_shared<ValidationError>(
                editor, tr("Invalid data entered"),
                QString(tr("The student with index number %1 already exists in "
                           "the system."))
                    .arg(text));
        }
    }
    return nullptr;
}

shared_ptr<ValidationError> StudentValidator::validateYearOfStudy() const {
    auto editor = dynamic_cast<QSpinBox *>(mFieldEditors["year_of_study"]);
    if (editor->value() < 1 || editor->value() > 7) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("Year of study must be between 1 and 7."));
    }

    return nullptr;
}
}
}
