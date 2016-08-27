#include "activityvalidator.h"

using namespace std;
using namespace paso::widget;

namespace paso {
namespace admin {

ActivityValidator::ActivityValidator(const FieldTypes &fieldTypes,
                                     const FieldEditors &fieldEditors,
                                     QObject *parent)
    : RecordValidator(fieldTypes, fieldEditors, parent) {}

ActivityValidator::~ActivityValidator() {}

shared_ptr<ValidationError>
ActivityValidator::validate(const QSqlRecord &original) const {
    // Validation is done in wizard dialog so we always report no errors here.
    return nullptr;
}
}
}
