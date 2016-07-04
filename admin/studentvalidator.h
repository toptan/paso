#ifndef STUDENTVALIDATOR_H
#define STUDENTVALIDATOR_H

#include "recordvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

class StudentValidator : public RecordValidator {
    Q_OBJECT
public:
    StudentValidator(const FieldTypes &fieldTypes,
                     const FieldEditors &fieldEditors,
                     QObject *parent = nullptr);
    virtual ~StudentValidator() {}

    virtual std::shared_ptr<ValidationError>
    validate(const QSqlRecord &original) const override;
};
}
}

#endif // STUDENTVALIDATOR_H
