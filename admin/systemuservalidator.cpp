#include "systemuservalidator.h"

#include <QVariant>
#include <QStyle>
#include <QMessageBox>

namespace paso {
namespace admin {
SystemUserValidator::SystemUserValidator(const FieldTypes &fieldTypes,
                                         const FieldEditors &fieldEditors,
                                         QObject *parent)
    : RecordValidator(fieldTypes, fieldEditors, parent) {}

bool SystemUserValidator::validate() const {
    for (auto &editor : mFieldEditors.values()) {
        editor->setProperty("error", true);
        editor->style()->unpolish(editor);
        editor->style()->polish(editor);
    }
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Invalid data entered"));
    msgBox.setText(tr("Some of the values for the system user are invalid."));
    msgBox.setDetailedText(tr("Foo\nBar\nBaz"));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.exec();
    for (auto &editor : mFieldEditors.values()) {
        editor->setProperty("error", false);
        editor->style()->unpolish(editor);
        editor->style()->polish(editor);
    }
    return false;
}
}
}
