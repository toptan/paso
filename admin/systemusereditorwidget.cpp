#include "systemusereditorwidget.h"

#include "data.h"

#include <QApplication>
#include <QComboBox>
#include <QWidget>

using namespace paso::data;
using namespace paso::widget;

namespace paso {
namespace admin {

SystemUserEditorWidget::SystemUserEditorWidget(const FieldTypes &fieldTypes,
                                               QWidget *parent)
    : RecordEditorWidget(fieldTypes, parent), mEditingRootSystemUser(false) {}

void SystemUserEditorWidget::prepareEdit(QSqlRecord &record) {
    mEditingRootSystemUser = record.value("username") == "root";
}

bool SystemUserEditorWidget::fieldReadOnly(const QString &key) {
    return key == "username" && mEditingRootSystemUser;
}

QComboBox *SystemUserEditorWidget::createComboBox(const QString &field) {
    auto retVal = RecordEditorWidget::createComboBox(field);
    for (const auto &role : enumeratedRoles.keys()) {
        retVal->addItem(
            QApplication::instance()->translate(
                "QObject", enumeratedRoles[role].toStdString().c_str()),
            roleToString(role));
    }
    return retVal;
}

QLineEdit *SystemUserEditorWidget::createLineEdit(const QString &field) {
    auto retVal = RecordEditorWidget::createLineEdit(field);
    if (field == "username") {
        retVal->setMaxLength(16);
    } else if (field == "first_name" || field == "last_name") {
        retVal->setMaxLength(32);
    } else if (field == "email") {
        retVal->setMaxLength(64);
    }
    return retVal;
}

QLineEdit *
SystemUserEditorWidget::createPasswordLineEdit(const QString &field) {
    auto retVal = RecordEditorWidget::createPasswordLineEdit(field);
    if (field == "password") {
        retVal->setMaxLength(32);
    }
    return retVal;
}
}
}
