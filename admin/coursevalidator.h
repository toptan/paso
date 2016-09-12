#ifndef COURSEVALIDATOR_H
#define COURSEVALIDATOR_H

#include "pasodb.h"
#include "recordvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

///
/// \brief The CourseValidator class is a class that validates course entry.
///
class CourseValidator : public paso::widget::RecordValidator {
    Q_OBJECT
public:
    ///
    /// \brief Constructs new course validator.
    /// \param fieldTypes Record field types.
    /// \param fieldEditors Record field editors.
    /// \param parent The parent widget.
    ///
    CourseValidator(const paso::widget::FieldTypes &fieldTypes,
                    const paso::widget::FieldEditors &fieldEditors,
                    QObject *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
    virtual ~CourseValidator();

    ///
    /// \brief Validates entered course data.
    /// \param original The original record, before modifications, to use for
    /// additional checking.
    /// \return A validation error structure with the data of the first
    /// validation check that failed or \c nullptr if no error.
    ///
    virtual std::shared_ptr<paso::widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    ///
    /// \brief Validates course code.
    /// \param original The original course code before the change.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError>
    validateCode(const QString &original) const;
    ///
    /// \brief Validates course name.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError> validateName() const;
};
}
}
#endif // COURSEVALIDATOR_H
