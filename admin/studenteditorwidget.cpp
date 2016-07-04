#include "studenteditorwidget.h"

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

QWidget *
StudentEditorWidget::createComboBoxForRecordField(const QString &field) {
    // TODO: Check if some fields should be combo boxes.
    return nullptr;
}
}
}
