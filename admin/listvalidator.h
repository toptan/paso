#ifndef LISTVALIDATOR_H
#define LISTVALIDATOR_H

#include "pasodb.h"
#include "recordvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

///
/// \brief The ListValidator class is a class that validates list entry.
///
class ListValidator : public paso::widget::RecordValidator {
    Q_OBJECT
public:
    ///
    /// \brief Constructs new list validator.
    /// \param fieldTypes Record field types.
    /// \param fieldEditors Record field editors.
    /// \param parent The parent widget.
    ///
    ListValidator(const paso::widget::FieldTypes &fieldTypes,
                  const paso::widget::FieldEditors &fieldEditors,
                  QObject *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
    virtual ~ListValidator();

    ///
    /// \brief Validates entered list data.
    /// \param original The original record, before modifications, to use for
    /// additional checking.
    /// \return A validation error structure with the data of the first
    /// validation check that failed or \c nullptr if no error.
    ///
    virtual std::shared_ptr<paso::widget::ValidationError>
    validate(const QSqlRecord &original) const override;

private:
    ///
    /// \brief Validates list name.
    /// \param original The original list name before the change.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError>
    validateName(const QString &original) const;
    ///
    /// \brief Validates list expiry date.
    /// \param original The original expiry date before the change.
    /// \return The validation error structure or \c nullptr if no error.
    ///
    std::shared_ptr<paso::widget::ValidationError>
    validateExpiryDate(const QDate &original) const;
};
}
}

#endif // LISTVALIDATOR_H
