#include "systemusereditorwidget.h"

#include "data.h"

#include <QComboBox>
#include <QWidget>

namespace paso {
namespace admin {

using namespace paso::data;

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

QWidget *
SystemUserEditorWidget::createComboBoxForRecordField(const QString &field) {
    if (field != "role") {
        return nullptr;
    }

    auto combo = new QComboBox(this);
    for (const auto &role : enumeratedRoles.keys()) {
        combo->addItem(enumeratedRoles[role], roleToString(role));
    }
    combo->setEnabled(false);
    combo->setCurrentIndex(-1);
    return combo;
}
}
}
