#ifndef ROOMVALIDATOR_H
#define ROOMVALIDATOR_H

#include "recordvalidator.h"
#include "pasodb.h"

#include <QObject>

namespace paso {
namespace admin {

class RoomValidator : public RecordValidator {
    Q_OBJECT
public:
    RoomValidator(const FieldTypes &fieldTypes,
                  const FieldEditors &fieldEditors, QObject *parent = nullptr);
    virtual ~RoomValidator() {}

    virtual std::shared_ptr<ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    std::shared_ptr<ValidationError>
    validateRoomUuid(const QString &original) const;
    std::shared_ptr<ValidationError> validateName() const;
    std::shared_ptr<ValidationError>
    validateRoomNumber(const QString &original) const;
};
}
}

#endif // ROOMVALIDATOR_H
