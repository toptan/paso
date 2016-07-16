#ifndef PERSONVALIDATOR_H
#define PERSONVALIDATOR_H

#include "recordvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

class PersonValidator : public RecordValidator {
    Q_OBJECT
public:
    PersonValidator(const FieldTypes &fieldTypes,
                    const FieldEditors &fieldEditors,
                    QObject *parent = nullptr);
    virtual ~PersonValidator() {}

    virtual std::shared_ptr<ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    std::shared_ptr<ValidationError>
    validateFirstName() const;
    std::shared_ptr<ValidationError>
    validateLastName() const;
    std::shared_ptr<ValidationError>
    validateEmail() const;
};
}
}
#endif // PERSONVALIDATOR_H
