#ifndef MOCKALWAYSINVALIDRECORDVALIDATOR_H
#define MOCKALWAYSINVALIDRECORDVALIDATOR_H

#include "recordvalidator.h"
#include <memory>

using namespace paso::widget;
using namespace std;

class MockAlwaysInvalidRecordValidator : public RecordValidator {
public:
    MockAlwaysInvalidRecordValidator(QObject *parent = nullptr);

    virtual shared_ptr<ValidationError>
    validate(const QSqlRecord &original) const override;
};

#endif // MOCKALWAYSINVALIDRECORDVALIDATOR_H
