#ifndef RECORDVALIDATOR_H
#define RECORDVALIDATOR_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QSqlRecord>
#include <QWidget>

namespace paso {
namespace admin {

///
/// \brief The FieldType enum defines widget to be used for record fields.
///
enum class FieldType { LineEdit, PasswordEdit, ComboBox };

using FieldTypes = QMap<QString, FieldType>;
using FieldEditors = QMap<QString, QWidget *>;

///
/// \brief The RecordValidator class is a base abstract class for all record
/// validators.
///
class RecordValidator : public QObject {
public:
    explicit RecordValidator(const FieldTypes &fieldTypes,
                             const FieldEditors &fieldEditors,
                             QObject *parent = 0)
        : QObject(parent), mFieldTypes(fieldTypes),
          mFieldEditors(fieldEditors) {}
    virtual ~RecordValidator() {}

    ///
    /// \brief validate Validates entered data.
    /// \param original The original record, before modifications, to use for
    /// additional checking.
    /// \return \code true if all entered values are valid.
    ///
    virtual bool validate(const QSqlRecord &original) const = 0;

protected:
    const FieldTypes &mFieldTypes;
    const FieldEditors &mFieldEditors;
};
}
}

#endif // RECORDVALIDATOR_H
