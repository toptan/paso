#ifndef MOCKALWAYSVALIDRECORDVALIDATOR_H
#define MOCKALWAYSVALIDRECORDVALIDATOR_H

#include "recordvalidator.h"
#include <memory>

using namespace paso::widget;
using namespace std;

class MockAlwaysValidRecordValidator : public RecordValidator {
public:
    MockAlwaysValidRecordValidator(QObject *parent = nullptr);

    virtual shared_ptr<ValidationError>
    validate(const QSqlRecord &original) const override;
};

#endif // MOCKALWAYSVALIDRECORDVALIDATOR_H
