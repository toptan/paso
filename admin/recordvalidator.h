#ifndef RECORDVALIDATOR_H
#define RECORDVALIDATOR_H

#include "pasodb.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QSqlRecord>
#include <QWidget>
#include <QStyle>
#include <QVariant>
#include <QMessageBox>
#include <memory>

namespace paso {
namespace admin {

///
/// \brief The FieldType enum defines widget to be used for record fields.
///
enum class FieldType {
    LineEdit,
    MaskedLineEdit,
    PasswordEdit,
    NumberEdit,
    ComboBox
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
    explicit RecordValidator(const FieldTypes &fieldTypes,
                             const FieldEditors &fieldEditors,
                             QObject *parent = 0)
        : QObject(parent), mFieldTypes(fieldTypes), mFieldEditors(fieldEditors),
          mDbManager() {}
    virtual ~RecordValidator() {}

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

    const db::DBManager &dbManager() const { return mDbManager; }

private:
    db::DBManager mDbManager;
};

static void setEntryError(QWidget *widget, bool error) {}

static void showEntryError(QWidget *editor, const QString &title,
                           const QString &text,
                           const QString &detailedText = "",
                           QMessageBox::Icon icon = QMessageBox::NoIcon) {
    if (editor != nullptr) {
        editor->setProperty("error", true);
        editor->style()->unpolish(editor);
        editor->style()->polish(editor);
    }
    QMessageBox msgBox(editor);
    msgBox.setIcon(icon);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setWindowModality(Qt::WindowModal);
    if (!detailedText.isEmpty()) {
        msgBox.setDetailedText(text);
    }
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    if (editor != nullptr) {
        editor->setProperty("error", false);
        editor->style()->unpolish(editor);
        editor->style()->polish(editor);
        editor->setFocus();
    }
}
}
}

#endif // RECORDVALIDATOR_H
