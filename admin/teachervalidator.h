#ifndef TEACHERVALIDATOR_H
#define TEACHERVALIDATOR_H

#include "pasodb.h"
#include "personvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

///
/// \brief The TeacherValidator class is a class that validates teacher entry.
///
class TeacherValidator : public PersonValidator {
    Q_OBJECT
public:
    ///
    /// \brief Contructs new teacher validator.
    /// \param fieldTypes Record field types.
    /// \param fieldEditors Record field editors.
    /// \param parent The parent widget.
    ///
    TeacherValidator(const widget::FieldTypes &fieldTypes,
                     const widget::FieldEditors &fieldEditors,
                     QObject *parent = nullptr);

    ///
    /// \brief Destructor.
    ///
    virtual ~TeacherValidator();

    ///
    /// \brief Validates teacher's data.
    /// \param original The original record, before modifications, to use for
    /// additional checking.
    /// \return A validation error structure with the data of the first
    /// validation check that failed or \c nullptr if no error.
    ///
    virtual std::shared_ptr<widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    ///
    /// \brief Validates teacher's employee number.
    /// \param original The original employee number before change.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<widget::ValidationError>
    validateEmployeeNumber(const QString &original) const;
    ///
    /// \brief Validates teacher's office number.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<widget::ValidationError> validateOfficeNumber() const;
};
}
}

#endif // TEACHERVALIDATOR_H
