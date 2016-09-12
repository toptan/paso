#ifndef STUDENTVALIDATOR_H
#define STUDENTVALIDATOR_H

#include "pasodb.h"
#include "personvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

///
/// \brief The StudentValidator class is a class that validates student entry.
///
class StudentValidator : public PersonValidator {
    Q_OBJECT
public:
    ///
    /// \brief Constructs new student validator.
    /// \param fieldTypes Record field types.
    /// \param fieldEditors Record field editors.
    /// \param parent The parent widget.
    ///
    StudentValidator(const widget::FieldTypes &fieldTypes,
                     const widget::FieldEditors &fieldEditors,
                     QObject *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
    virtual ~StudentValidator();

    ///
    /// \brief Validates student's data.
    /// \param original The original record, before modifications, to use for
    /// additional checking.
    /// \return A validation error structure with the data of the first
    /// validation check that failed or \c nullptr if no error.
    ///
    virtual std::shared_ptr<widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    ///
    /// \brief Validates student's index number.
    /// \param original The original index number before change.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<widget::ValidationError>
    validateIndexNumber(const QString &original) const;
    ///
    /// \brief Validates student's year of study.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<widget::ValidationError> validateYearOfStudy() const;
};
}
}

#endif // STUDENTVALIDATOR_H
