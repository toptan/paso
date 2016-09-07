#include "teachereditorwidget.h"

using namespace paso::widget;

namespace paso {
namespace admin {

TeacherEditorWidget::TeacherEditorWidget(const FieldTypes &fieldTypes,
                                         QWidget *parent)
    : RecordEditorWidget(fieldTypes, parent) {}

void TeacherEditorWidget::prepareEdit(QSqlRecord &record) {
    // Nothing to prepare
}

bool TeacherEditorWidget::fieldReadOnly(const QString &key) {
    // All fields are editable.
    return false;
}

QLineEdit *TeacherEditorWidget::createLineEdit(const QString &field) {
    auto retVal = RecordEditorWidget::createLineEdit(field);
    if (field == "first_name") {
        retVal->setMaxLength(32);
    } else if (field == "last_name") {
        retVal->setMaxLength(32);
    } else if (field == "email") {
        retVal->setMaxLength(64);
    } else if (field == "rfid") {
        retVal->setMaxLength(64);
    } else if (field == "employee_number") {
        retVal->setMaxLength(16);
    } else if (field == "office") {
        retVal->setMaxLength(32);
    }
    return retVal;
}
}
}
