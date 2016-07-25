#include "listvalidator.h"

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
    return nullptr;
}
}
}
