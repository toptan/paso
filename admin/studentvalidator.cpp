#include "studentvalidator.h"

namespace paso {
namespace admin {

using namespace std;

StudentValidator::StudentValidator(const FieldTypes &fieldTypes,
                                   const FieldEditors &fieldEditors,
                                   QObject *parent)
    : RecordValidator(fieldTypes, fieldEditors, parent) {}

shared_ptr<ValidationError>
StudentValidator::validate(const QSqlRecord &original) const {
    // TODO: Implement student record validation.
    return nullptr;
}
}
}
