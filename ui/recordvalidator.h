#ifndef RECORDVALIDATOR_H
#define RECORDVALIDATOR_H

#include "pasodb.h"

#include <QMap>
#include <QMessageBox>
#include <QObject>
#include <QSqlRecord>
#include <QString>
#include <QStringList>
#include <QStyle>
#include <QVariant>
#include <QWidget>
#include <memory>

namespace paso {
namespace widget {

///
/// \brief The FieldType enum defines widget to be used for record fields.
///
enum class FieldType {
    LineEdit,
    MaskedLineEdit,
    PasswordEdit,
    NumberEdit,
    ComboBox,
    CheckBox,
    DateEdit,
    TimeEdit
};

using FieldTypes = QMap<QString, FieldType>;
using FieldEditors = QMap<QString, QWidget *>;

///
/// \brief The ValidationError struct is a message container for validation
/// errors.
///
struct ValidationError {

    explicit ValidationError(QWidget *anEditor, const QString &aTitle,
                             const QString &aText,
                             const QString &aDetailedText = "",
                             QMessageBox::Icon anIcon = QMessageBox::NoIcon)
        : editor(anEditor), title(aTitle), text(aText),
          detailedText(aDetailedText), icon(anIcon) {}

    QWidget *editor;
    const QString title;
    const QString text;
    const QString detailedText;
    const QMessageBox::Icon icon;
};

///
/// \brief The RecordValidator class is a base abstract class for all record
/// validators.
///
class RecordValidator : public QObject {
public:
    RecordValidator(const FieldTypes &fieldTypes,
                    const FieldEditors &fieldEditors,
                    QObject *parent = nullptr);
    virtual ~RecordValidator();

    ///
    /// \brief validate Validates entered data.
    /// \param original The original record, before modifications, to use for
    /// additional checking.
    /// \return A validation error structure with the data of the first
    /// validation check that failed.
    ///
    virtual std::shared_ptr<ValidationError>
    validate(const QSqlRecord &original) const = 0;

protected:
    const FieldTypes &mFieldTypes;
    const FieldEditors &mFieldEditors;

    const db::DBManager &dbManager() const;

private:
    db::DBManager mDbManager;
};

void showEntryError(QWidget *editor, const QString &title, const QString &text,
                    const QString &detailedText = "",
                    QMessageBox::Icon icon = QMessageBox::NoIcon);
}
}

#endif // RECORDVALIDATOR_H
