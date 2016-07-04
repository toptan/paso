#include "courseeditorwidget.h"

namespace paso {
namespace admin {

CourseEditorWidget::CourseEditorWidget(const QSqlRecord &record,
                                       const FieldTypes &fieldTypes,
                                       QWidget *parent)
    : RecordEditorWidget(record, fieldTypes, parent) {}

void CourseEditorWidget::prepareEdit(QSqlRecord &record) {
    // Nothing to do
}

bool CourseEditorWidget::fieldReadOnly(const QString &) {
    // All fields are editable
    return false;
}

QWidget *
CourseEditorWidget::createComboBoxForRecordField(const QString &field) {
    // No field requires combo box for editing.
    return nullptr;
}
}
}
