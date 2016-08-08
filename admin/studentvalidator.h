#ifndef STUDENTVALIDATOR_H
#define STUDENTVALIDATOR_H

#include "personvalidator.h"
#include "pasodb.h"

#include <QObject>

namespace paso {
namespace admin {

class StudentValidator : public PersonValidator {
    Q_OBJECT
public:
    StudentValidator(const widget::FieldTypes &fieldTypes,
                     const widget::FieldEditors &fieldEditors,
                     QObject *parent = nullptr);
    virtual ~StudentValidator();

    virtual std::shared_ptr<widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    std::shared_ptr<widget::ValidationError>
    validateIndexNumber(const QString &original) const;
    std::shared_ptr<widget::ValidationError> validateYearOfStudy() const;
};
}
}

#endif // STUDENTVALIDATOR_H
