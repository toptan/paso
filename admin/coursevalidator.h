#ifndef COURSEVALIDATOR_H
#define COURSEVALIDATOR_H

#include "recordvalidator.h"
#include "pasodb.h"

#include <QObject>

namespace paso {
namespace admin {

class CourseValidator : public RecordValidator {
    Q_OBJECT
public:
    CourseValidator(const FieldTypes &fieldTypes,
                    const FieldEditors &fieldEditors,
                    QObject *parent = nullptr);
    virtual ~CourseValidator() {}

    virtual std::shared_ptr<ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    std::shared_ptr<ValidationError>
    validateCode(const QString &original) const;
    std::shared_ptr<ValidationError> validateName() const;
};
}
}
#endif // COURSEVALIDATOR_H
