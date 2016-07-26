#include "listvalidator.h"

#include <QCheckBox>
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
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["name"]);
    auto text = editor->text().trimmed();
    auto originalText = original.value("name").toString();

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

    if (originalText.isEmpty() || text != originalText) {
        QSqlError error;
        const auto unique = dbManager().listNameUnique(text, error);
        if (error.type() != QSqlError::NoError) {
            return make_shared<ValidationError>(
                nullptr, tr("Critical error"),
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
}
}
