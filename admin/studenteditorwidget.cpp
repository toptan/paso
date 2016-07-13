#include "studenteditorwidget.h"

#include <QDebug>

namespace paso {
namespace admin {

StudentEditorWidget::StudentEditorWidget(const QSqlRecord &record,
                                         const FieldTypes &fieldTypes,
                                         QWidget *parent)
    : RecordEditorWidget(record, fieldTypes, parent) {}

void StudentEditorWidget::prepareEdit(QSqlRecord &record) {
    // TODO: Prepare student record for editing.
}

bool StudentEditorWidget::fieldReadOnly(const QString &key) {
    // TODO: Check if all fields are editable.
    return false;
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
