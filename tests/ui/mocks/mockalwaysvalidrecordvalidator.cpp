#include "mockalwaysvalidrecordvalidator.h"

MockAlwaysValidRecordValidator::MockAlwaysValidRecordValidator(QObject *parent)
    : RecordValidator(FieldTypes{}, FieldEditors{}, parent) {}

shared_ptr<ValidationError>
MockAlwaysValidRecordValidator::validate(const QSqlRecord &) const {
    return nullptr;
}
