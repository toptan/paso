#ifndef PERSONVALIDATOR_H
#define PERSONVALIDATOR_H

#include "recordvalidator.h"

#include <QObject>
#include <QSqlRecord>
#include <memory>

namespace paso {
namespace admin {

///
/// \brief The PersonValidator class is a class that validates common data for
/// students and teachers.
///
class PersonValidator : public paso::widget::RecordValidator {
    Q_OBJECT
public:
    ///
    /// \brief Contructs person validator.
    /// \param fieldTypes Record field types.
    /// \param fieldEditors Record field editors.
    /// \param parent The parent widget.
    ///
    PersonValidator(const paso::widget::FieldTypes &fieldTypes,
                    const paso::widget::FieldEditors &fieldEditors,
                    QObject *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
    virtual ~PersonValidator();

    ///
    /// \brief Validates person's data.
    /// \param original The original record, before modifications, to use for
    /// additional checking.
    /// \return A validation error structure with the data of the first
    /// validation check that failed or \c nullptr if no error.
    ///
    virtual std::shared_ptr<paso::widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    ///
    /// \brief Validates person's first name.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError> validateFirstName() const;
    ///
    /// \brief Validates person's last name.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError> validateLastName() const;
    ///
    /// \brief Validates person's email.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError> validateEmail() const;
};
}
}
#endif // PERSONVALIDATOR_H
