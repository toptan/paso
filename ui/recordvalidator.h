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
/// \brief The FieldType enum defines editor widget to be used for record
/// fields.
///
enum class FieldType {
    LineEdit,       //!< Plain line edit.
    MaskedLineEdit, //!< Masked line edit.
    PasswordEdit,   //!< The password line edit.
    NumberEdit,     //!< The spinner edit.
    ComboBox,       //!< Combo box.
    CheckBox,       //!< Check box.
    DateEdit,       //!< Date picker.
    TimeEdit        //!< Time picker.
};

using FieldTypes = QMap<QString, FieldType>;
using FieldEditors = QMap<QString, QWidget *>;

///
/// \brief The ValidationError struct is a message container for validation
/// errors.
///
struct ValidationError {

    ///
    /// \brief Constructs validation error for given editor widget and the data.
    /// \param anEditor The editor widget to attach validator to.
    /// \param aTitle The title of an error message.
    /// \param aText The text of an error message.
    /// \param aDetailedText The detailed text of an error message if any.
    /// \param anIcon The icon to use to display message box.
    ///
    explicit ValidationError(QWidget *anEditor, const QString &aTitle,
                             const QString &aText,
                             const QString &aDetailedText = "",
                             QMessageBox::Icon anIcon = QMessageBox::NoIcon)
        : editor(anEditor), title(aTitle), text(aText),
          detailedText(aDetailedText), icon(anIcon) {}

    QWidget *editor;              //!< The editor widget to attach message to.
    const QString title;          //!< The message title.
    const QString text;           //!< The message text.
    const QString detailedText;   //!< Detailed message text if available.
    const QMessageBox::Icon icon; //!< The icon to use when showing message box.
};

///
/// \brief The RecordValidator class is a base abstract class for all record
/// validators.
///
class RecordValidator : public QObject {
public:
    ///
    /// \brief Contructs new record validator.
    /// \param fieldTypes Record field types.
    /// \param fieldEditors Record field editors.
    /// \param parent The parent widget.
    ///
    RecordValidator(const FieldTypes &fieldTypes,
                    const FieldEditors &fieldEditors,
                    QObject *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
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
    const FieldTypes &mFieldTypes;     //!< The record field types.
    const FieldEditors &mFieldEditors; //!< The record field editors.

    ///
    /// \brief Returns the database manager to use.
    /// \return Reference to the database manager.
    ///
    const db::DBManager &dbManager() const;

private:
    db::DBManager mDbManager; //!< The database manager to use.
};

///
/// \brief Shows message box attached to editor with error message.
/// \param editor The editor to attach to.
/// \param title The message box title.
/// \param text The message text.
/// \param detailedText Detailed text if availab.e
/// \param icon The icon to use in message box.
///
void showEntryError(QWidget *editor, const QString &title, const QString &text,
                    const QString &detailedText = "",
                    QMessageBox::Icon icon = QMessageBox::NoIcon);
}
}

#endif // RECORDVALIDATOR_H
