#ifndef ROOMVALIDATOR_H
#define ROOMVALIDATOR_H

#include "recordvalidator.h"
#include "pasodb.h"

#include <QObject>

namespace paso {
namespace admin {

class RoomValidator : public widget::RecordValidator {
    Q_OBJECT
public:
    RoomValidator(const widget::FieldTypes &fieldTypes,
                  const widget::FieldEditors &fieldEditors, QObject *parent = nullptr);
    virtual ~RoomValidator();

    virtual std::shared_ptr<widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    std::shared_ptr<widget::ValidationError>
    validateRoomUuid(const QString &original) const;
    std::shared_ptr<widget::ValidationError> validateName() const;
    std::shared_ptr<widget::ValidationError>
    validateRoomNumber(const QString &original) const;
};
}
}

#endif // ROOMVALIDATOR_H
