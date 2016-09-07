#ifndef TEACHERVALIDATOR_H
#define TEACHERVALIDATOR_H

#include "pasodb.h"
#include "personvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

class TeacherValidator : public PersonValidator {
    Q_OBJECT
public:
    TeacherValidator(const widget::FieldTypes &fieldTypes,
                     const widget::FieldEditors &fieldEditors,
                     QObject *parent = nullptr);

    virtual ~TeacherValidator();

    virtual std::shared_ptr<widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    std::shared_ptr<widget::ValidationError>
    validateEmployeeNumber(const QString &original) const;
    std::shared_ptr<widget::ValidationError> validateOfficeNumber() const;
};
}
}

#endif // TEACHERVALIDATOR_H
