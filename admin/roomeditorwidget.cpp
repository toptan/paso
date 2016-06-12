#include "roomeditorwidget.h"

#include <QSqlField>
#include <QUuid>

namespace paso {
namespace admin {
RoomEditorWidget::RoomEditorWidget(const QSqlRecord &record,
                                   const FieldTypes &fieldTypes,
                                   QWidget *parent)
    : RecordEditorWidget(record, fieldTypes, parent) {}

void RoomEditorWidget::prepareEdit(QSqlRecord &record) {
    if (record.field("room_uuid").isNull()) {
        record.setValue("room_uuid", QUuid::createUuid());
    }
}

bool RoomEditorWidget::fieldReadOnly(const QString &key) {
    return key == "room_uuid";
}
}
}
