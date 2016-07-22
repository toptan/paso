#ifndef SYSTEMUSERVALIDATOR_H
#define SYSTEMUSERVALIDATOR_H

#include "recordvalidator.h"
#include "pasodb.h"

#include <QObject>

namespace paso {
namespace admin {

class SystemUserValidator : public widget::RecordValidator {
    Q_OBJECT
public:
    SystemUserValidator(const widget::FieldTypes &fieldTypes,
                        const widget::FieldEditors &fieldEditors,
                        QObject *parent = nullptr);
    virtual ~SystemUserValidator() {}

    virtual std::shared_ptr<widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    std::shared_ptr<widget::ValidationError>
    validateUsername(const QString &original) const;
    std::shared_ptr<widget::ValidationError> validatePassword() const;
    std::shared_ptr<widget::ValidationError> validateFirstName() const;
    std::shared_ptr<widget::ValidationError> validateLastName() const;
    std::shared_ptr<widget::ValidationError> validateEmail() const;
};
}
}
#endif // SYSTEMUSERVALIDATOR_H
