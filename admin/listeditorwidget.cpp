#include "listeditorwidget.h"

using namespace paso::widget;

namespace paso {
namespace admin {

ListEditorWidget::ListEditorWidget(const widget::FieldTypes &fieldTypes,
                                   QWidget *parent)
    : RecordEditorWidget(fieldTypes, parent) {}

void ListEditorWidget::prepareEdit(QSqlRecord &record) {
    // Nothing to do.
}

bool ListEditorWidget::fieldReadOnly(const QString &key) {
    return key == "system";
}

QLineEdit *ListEditorWidget::createLineEdit(const QString &field) {
    auto retVal = RecordEditorWidget::createLineEdit(field);
    if (field == "name") {
        retVal->setMaxLength(64);
    }

    return retVal;
}

}
}
