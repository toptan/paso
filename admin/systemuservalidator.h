#ifndef SYSTEMUSERVALIDATOR_H
#define SYSTEMUSERVALIDATOR_H

#include "pasodb.h"
#include "recordvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

///
/// \brief The SystemUserValidator class is a class that validates system user
/// entry.
///
class SystemUserValidator : public widget::RecordValidator {
    Q_OBJECT
public:
    ///
    /// \brief Constructs new system user validator.
    /// \param fieldTypes Record field types.
    /// \param fieldEditors Record field editors.
    /// \param parent The parent widget.
    ///
    SystemUserValidator(const paso::widget::FieldTypes &fieldTypes,
                        const paso::widget::FieldEditors &fieldEditors,
                        QObject *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
    virtual ~SystemUserValidator();

    ///
    /// \brief Validates entered system user data.
    /// \param original The original record, before modifications, to use for
    /// additional checking.
    /// \return A validation error structure with the data of the first
    /// validation check that failed or \c nullptr if no error.
    ///
    virtual std::shared_ptr<paso::widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    ///
    /// \brief Validates username.
    /// \param original The original username before the change.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError>
    validateUsername(const QString &original) const;
    ///
    /// \brief Validates password.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError> validatePassword() const;
    ///
    /// \brief Validates first name.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError> validateFirstName() const;
    ///
    /// \brief Validates last name.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError> validateLastName() const;
    ///
    /// \brief Validates email.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError> validateEmail() const;
};
}
}
#endif // SYSTEMUSERVALIDATOR_H
