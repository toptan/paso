#include "systemusereditorwidget.h"

namespace paso {
namespace admin {

SystemUserEditorWidget::SystemUserEditorWidget(const QSqlRecord &record,
                                               const FieldTypes &fieldTypes,
                                               QWidget *parent)
    : RecordEditorWidget(record, fieldTypes, parent),
      mEditingRootSystemUser(false) {}

void SystemUserEditorWidget::prepareEdit(QSqlRecord &record) {
    mEditingRootSystemUser = record.value("username") == "root";
}

bool SystemUserEditorWidget::fieldReadOnly(const QString &key) {
    return key == "username" && mEditingRootSystemUser;
}
}
}
