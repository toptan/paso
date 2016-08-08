#ifndef PERSONVALIDATOR_H
#define PERSONVALIDATOR_H

#include "recordvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

class PersonValidator : public widget::RecordValidator {
    Q_OBJECT
public:
    PersonValidator(const widget::FieldTypes &fieldTypes,
                    const widget::FieldEditors &fieldEditors,
                    QObject *parent = nullptr);
    virtual ~PersonValidator();

    virtual std::shared_ptr<widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    std::shared_ptr<widget::ValidationError>
    validateFirstName() const;
    std::shared_ptr<widget::ValidationError>
    validateLastName() const;
    std::shared_ptr<widget::ValidationError>
    validateEmail() const;
};
}
}
#endif // PERSONVALIDATOR_H
