#include "recordvalidator.h"

using namespace paso::db;

namespace paso {
namespace widget {

RecordValidator::RecordValidator(const FieldTypes &fieldTypes,
                                 const FieldEditors &fieldEditors,
                                 QObject *parent)
    : QObject(parent), mFieldTypes(fieldTypes), mFieldEditors(fieldEditors),
      mDbManager() {}

RecordValidator::~RecordValidator() {}

const DBManager &RecordValidator::dbManager() const { return mDbManager; }

void showEntryError(QWidget *editor, const QString &title, const QString &text,
                    const QString &detailedText, QMessageBox::Icon icon) {
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
        msgBox.setDetailedText(detailedText);
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
