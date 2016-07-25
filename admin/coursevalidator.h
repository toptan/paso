#ifndef COURSEVALIDATOR_H
#define COURSEVALIDATOR_H

#include "recordvalidator.h"
#include "pasodb.h"

#include <QObject>

namespace paso {
namespace admin {

class CourseValidator : public widget::RecordValidator {
    Q_OBJECT
public:
    CourseValidator(const widget::FieldTypes &fieldTypes,
                    const widget::FieldEditors &fieldEditors,
                    QObject *parent = nullptr);
    virtual ~CourseValidator();

    virtual std::shared_ptr<widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    std::shared_ptr<widget::ValidationError>
    validateCode(const QString &original) const;
    std::shared_ptr<widget::ValidationError> validateName() const;
};
}
}
#endif // COURSEVALIDATOR_H
