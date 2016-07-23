#include "studenteditorwidget.h"

using namespace paso::widget;

namespace paso {
namespace admin {

StudentEditorWidget::StudentEditorWidget(const FieldTypes &fieldTypes,
                                         QWidget *parent)
    : RecordEditorWidget(fieldTypes, parent) {}

void StudentEditorWidget::prepareEdit(QSqlRecord &) {
    // Nothing to prepare.
}

bool StudentEditorWidget::fieldReadOnly(const QString &key) {
    // All fields are editable.
    return false;
}

void StudentEditorWidget::saveError() {
    // No action needed.
}

QLineEdit *StudentEditorWidget::createLineEdit(const QString &field) {
    auto retVal = RecordEditorWidget::createLineEdit(field);
    if (field == "first_name") {
        retVal->setMaxLength(32);
    } else if (field == "last_name") {
        retVal->setMaxLength(32);
    } else if (field == "email") {
        retVal->setMaxLength(64);
    } else if (field == "rfid") {
        retVal->setMaxLength(64);
    }
    return retVal;
}

QLineEdit *StudentEditorWidget::createMaskedLineEdit(const QString &field) {
    auto retVal = RecordEditorWidget::createMaskedLineEdit(field);
    if (field == "index_number") {
        retVal->setMaxLength(9);
        retVal->setInputMask("9999/9999");
    }

    return retVal;
}

QSpinBox *StudentEditorWidget::createSpinBox(const QString &field) {
    auto retVal = RecordEditorWidget::createSpinBox(field);
    if (field == "year_of_study") {
        retVal->setMinimum(1);
        retVal->setMaximum(7);
    }
    return retVal;
}
}
}
