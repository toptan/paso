#ifndef ROOMVALIDATOR_H
#define ROOMVALIDATOR_H

#include "pasodb.h"
#include "recordvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

///
/// \brief The RoomValidator class is a class that validates room data entry.
///
class RoomValidator : public paso::widget::RecordValidator {
    Q_OBJECT
public:
    ///
    /// \brief Constructs new room validator.
    /// \param fieldTypes Record field types.
    /// \param fieldEditors Record field editors.
    /// \param parent The parent widget.
    ///
    RoomValidator(const paso::widget::FieldTypes &fieldTypes,
                  const paso::widget::FieldEditors &fieldEditors,
                  QObject *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
    virtual ~RoomValidator();

    ///
    /// \brief Validates entered room data.
    /// \param original The original record, before modifications, to use for
    /// additional checking.
    /// \return A validation error structure with the data of the first
    /// validation check that failed or \c nullptr if no error.
    ///
    virtual std::shared_ptr<paso::widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    ///
    /// \brief Validates room UUID.
    /// \param original The original room UUID before the change.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError>
    validateRoomUuid(const QString &original) const;
    ///
    /// \brief Validates room name.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError> validateName() const;
    ///
    /// \brief Validates room number.
    /// \param original The original room number before the change.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError>
    validateRoomNumber(const QString &original) const;
};
}
}

#endif // ROOMVALIDATOR_H
