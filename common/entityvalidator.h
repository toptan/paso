#ifndef ENTITYVALIDATOR_H
#define ENTITYVALIDATOR_H

#include "jsonserializable.h"

#include <memory>
#include <QObject>

namespace paso {
namespace data {
namespace validator {

struct ValidationError {
    explicit ValidationError(const QString &aTitle, const QString &aText,
                             const QString &aDetailedText = "")
        : title(aTitle), text(aText), detailedText(aDetailedText) {}

    const QString title;
    const QString text;
    const QString detailedText;
};

///
/// \brief The EntityValidator class is a base abstract class for all entity
/// validators
///
class EntityValidator : public QObject {
public:
    ///
    /// \brief EntityValidator The constructor.
    /// \param parent The parent object.
    ///
    explicit EntityValidator(QObject *parent = nullptr);

    ///
    /// \brief validate Performs actual validation, and returns the first
    /// violation. If we are checking new entity, the pointer to original should be
    /// \param current Object with current chagnes.
    /// \param original The original object.
    /// \return The validation error or \code nullptr if there are no
    /// violations.
    ///
    virtual std::shared_ptr<ValidationError>
    validate(const JsonSerializable &current,
             const JsonSerializable *original = nullptr) const = 0;

private:
};
}
}
}

#endif // ENTITYVALIDATOR_H
