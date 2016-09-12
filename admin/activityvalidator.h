#ifndef ACTIVITYVALIDATOR_H
#define ACTIVITYVALIDATOR_H

#include "recordvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

///
/// \brief The ActivityValidator class is a dummy class that validates activity
/// data. It always returns that there are no errors because activity data entry
/// is handled by \ref ActivityWizard.
///
class ActivityValidator : public paso::widget::RecordValidator {
    Q_OBJECT

public:
    ///
    /// \brief Constructs new activity validator.
    /// \param fieldTypes Record field types.
    /// \param fieldEditors Record field editors.
    /// \param parent The parent widget.
    ///
    ActivityValidator(const paso::widget::FieldTypes &fieldTypes,
                      const paso::widget::FieldEditors &fieldEditors,
                      QObject *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
    virtual ~ActivityValidator();

    ///
    /// \brief Validates activity data. Validation never fails as explained in
    /// class description.
    /// \param original The original record, before modifications, to use for
    /// additional checking.
    /// \return Always \c nullptr.
    ///
    virtual std::shared_ptr<paso::widget::ValidationError>
    validate(const QSqlRecord &original) const override;
};
}
}

#endif // ACTIVITYVALIDATOR_H
