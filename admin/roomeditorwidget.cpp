#include "roomeditorwidget.h"

#include <QSqlField>
#include <QUuid>

using namespace paso::widget;

namespace paso {
namespace admin {

RoomEditorWidget::RoomEditorWidget(const FieldTypes &fieldTypes,
                                   QWidget *parent)
    : RecordEditorWidget(fieldTypes, parent) {}

void RoomEditorWidget::prepareEdit(QSqlRecord &record) {
    if (record.field("room_uuid").isNull()) {
        record.setValue("room_uuid", QUuid::createUuid());
    }
}

bool RoomEditorWidget::fieldReadOnly(const QString &key) {
    return key == "room_uuid";
}

QLineEdit *RoomEditorWidget::createLineEdit(const QString &field) {
    auto retVal = RecordEditorWidget::createLineEdit(field);
    if (field == "name") {
        retVal->setMaxLength(64);
    } else if (field == "room_number") {
        retVal->setMaxLength(8);
    }
    return retVal;
}
}
}
