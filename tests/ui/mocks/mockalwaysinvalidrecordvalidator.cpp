#include "mockalwaysinvalidrecordvalidator.h"

MockAlwaysInvalidRecordValidator::MockAlwaysInvalidRecordValidator(
    QObject *parent)
    : RecordValidator(FieldTypes{}, FieldEditors{}, parent) {}

shared_ptr<ValidationError>
MockAlwaysInvalidRecordValidator::validate(const QSqlRecord &) const {
    return make_shared<ValidationError>(nullptr, "Invalid data entered",
                                        "Nothing will ever be valid.",
                                        "Just give up!");
}
