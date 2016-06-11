#include "roomeditorwidget.h"

namespace paso {
namespace admin {
RoomEditorWidget::RoomEditorWidget(const QSqlRecord &record,
                                   const FieldTypes &fieldTypes,
                                   QWidget *parent)
    : RecordEditorWidget(record, fieldTypes, parent) {}

void RoomEditorWidget::prepareEdit(const QSqlRecord &) {}

bool RoomEditorWidget::fieldReadOnly(const QString &key) {
    return key == "room_uuid";
}
}
}
