#ifndef ACTIVITYVALIDATOR_H
#define ACTIVITYVALIDATOR_H

#include "recordvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

class ActivityValidator : public widget::RecordValidator {
    Q_OBJECT

public:
    ActivityValidator(const widget::FieldTypes &fieldTypes,
                      const widget::FieldEditors &fieldEditors,
                      QObject *parent = nullptr);
    virtual ~ActivityValidator();

    virtual std::shared_ptr<widget::ValidationError>
    validate(const QSqlRecord &original) const override;
};
}
}

#endif // ACTIVITYVALIDATOR_H
