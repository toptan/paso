#include "courseeditorwidget.h"

using namespace paso::widget;

namespace paso {
namespace admin {

CourseEditorWidget::CourseEditorWidget(const FieldTypes &fieldTypes,
                                       QWidget *parent)
    : RecordEditorWidget(fieldTypes, parent) {}

void CourseEditorWidget::prepareEdit(QSqlRecord &record) {
    // Nothing to do
}

bool CourseEditorWidget::fieldReadOnly(const QString &) {
    // All fields are editable
    return false;
}

QLineEdit *CourseEditorWidget::createLineEdit(const QString &field) {
    auto retVal = RecordEditorWidget::createLineEdit(field);
    if (field == "code") {
        retVal->setMaxLength(8);
    } else if (field == "name") {
        retVal->setMaxLength(64);
    }
    return retVal;
}
}
}
