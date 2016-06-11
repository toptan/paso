#include "systemuservalidator.h"

#include <QLineEdit>
#include <QVariant>
#include <QStyle>
#include <QSqlError>
#include <QMessageBox>
#include <QRegExp>
#include <QRegExpValidator>

namespace paso {
namespace admin {

void setEntryError(QWidget *widget, bool error) {
    widget->setProperty("error", error);
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
}

void showMessageBox(QWidget *editor, const QString &title, const QString &text,
                    const QString &detailedText = "",
                    QMessageBox::Icon icon = QMessageBox::NoIcon) {
    if (editor != nullptr) {
        setEntryError(editor, true);
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
        setEntryError(editor, false);
        editor->setFocus();
    }
}

SystemUserValidator::SystemUserValidator(const FieldTypes &fieldTypes,
                                         const FieldEditors &fieldEditors,
                                         QObject *parent)
    : RecordValidator(fieldTypes, fieldEditors, parent), dbManager() {}

bool SystemUserValidator::validate(const QSqlRecord &original) const {
    if (!validateUsername(original.value("username").toString())) {
        return false;
    }
    if (!validatePassword()) {
        return false;
    }
    if (!validateFirstName()) {
        return false;
    }
    if (!validateLastName()) {
        return false;
    }
    if (!validateEmail()) {
        return false;
    }
    return true;
}

bool SystemUserValidator::validateUsername(const QString &original) const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["username"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty() || text.size() < 4 || text.size() > 16) {
        showMessageBox(editor, tr("Invalid data entered"),
                       tr("The username has to be at least four characters "
                          "long and cannot be longer than sixteen."));

        return false;
    }
    if (original.isEmpty() || text != original) {
        // Check if entered username is unique.
        QSqlError error;
        auto unique = dbManager.usernameUnique(text, error);
        if (error.type() != QSqlError::NoError) {
            showMessageBox(nullptr, tr("Critical error"),
                           tr("There was an error working with the database."),
                           error.text(), QMessageBox::Critical);
            return false;
        }
        if (!unique) {
            showMessageBox(editor, tr("Invalid data entered"),
                           tr("The username you entered is not unique."));
            return false;
        }
    }
    return true;
}

bool SystemUserValidator::validatePassword() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["password"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty() || text.size() < 8 || text.size() > 16) {
        showMessageBox(editor, tr("Invalid data entered"),
                       tr("The password has to be at least eight characters "
                          "long and cannot be longer than sixteen."));
        return false;
    }
    return true;
}

bool SystemUserValidator::validateFirstName() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["first_name"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty()) {
        showMessageBox(editor, tr("Invalid data entered"),
                       tr("You need to specify user's first name."));
        return false;
    }
    if (text.size() > 32) {
        showMessageBox(
            editor, tr("Invalid data entered"),
            tr("The first name cannot be longer than 32 characters."));
        return false;
    }
    return true;
}

bool SystemUserValidator::validateLastName() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["last_name"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty()) {
        showMessageBox(editor, tr("Invalid data entered"),
                       tr("You need to specify user's last name."));
        return false;
    }
    if (text.size() > 32) {
        showMessageBox(
            editor, tr("Invalid data entered"),
            tr("The first name cannot be longer than 32 characters."));
        return false;
    }
    return true;
}

bool SystemUserValidator::validateEmail() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["email"]);
    auto text = editor->text().trimmed();
    QRegExp regExp("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b",
                   Qt::CaseInsensitive);
    QRegExpValidator validator(regExp);
    int pos = 0;
    if (validator.validate(text, pos) != QValidator::Acceptable) {
        showMessageBox(
            editor, tr("Invalid data entered"),
            tr("You need to provide a valid email address for the user."));
        return false;
    }
    return true;
}
}
}
