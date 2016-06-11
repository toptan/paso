#ifndef SYSTEMUSERVALIDATOR_H
#define SYSTEMUSERVALIDATOR_H

#include "recordvalidator.h"
#include "pasodb.h"

#include <QObject>

namespace paso {
namespace admin {

class SystemUserValidator : public RecordValidator {
    Q_OBJECT
public:
    SystemUserValidator(const FieldTypes &fieldTypes,
                        const FieldEditors &fieldEditors,
                        QObject *parent = nullptr);
    virtual ~SystemUserValidator() {}

    virtual std::shared_ptr<ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    db::DBManager dbManager;
    std::shared_ptr<ValidationError>
    validateUsername(const QString &original) const;
    std::shared_ptr<ValidationError> validatePassword() const;
    std::shared_ptr<ValidationError> validateFirstName() const;
    std::shared_ptr<ValidationError> validateLastName() const;
    std::shared_ptr<ValidationError> validateEmail() const;
};
}
}
#endif // SYSTEMUSERVALIDATOR_H
