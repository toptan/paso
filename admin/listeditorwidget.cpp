#include "listeditorwidget.h"

#include <QCheckBox>

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
    if (key == "system") {
        return true;
    }
    if (key == "expiry_date") {
        auto permanentEdit =
            dynamic_cast<QCheckBox *>(fieldEditors()["permanent"]);
        auto systemEdit = dynamic_cast<QCheckBox *>(fieldEditors()["system"]);
        if (permanentEdit->isChecked() || systemEdit->isChecked()) {
            return true;
        }
    }
    return false;
}

QLineEdit *ListEditorWidget::createLineEdit(const QString &field) {
    auto retVal = RecordEditorWidget::createLineEdit(field);
    if (field == "name") {
        retVal->setMaxLength(64);
    }

    return retVal;
}

QCheckBox *ListEditorWidget::createCheckBox(const QString &field) {
    auto retVal = RecordEditorWidget::createCheckBox(field);
    if (field == "permanent") {
        connect(retVal, &QCheckBox::toggled, [this](bool checked) {
            auto expiryEdit =
                dynamic_cast<QDateEdit *>(fieldEditors()["expiry_date"]);
            expiryEdit->setReadOnly(checked);
        });
    }

    return retVal;
}

QDateEdit *ListEditorWidget::createDateEdit(const QString &field) {
    auto retVal = RecordEditorWidget::createDateEdit(field);
    if (field == "expiry_date") {
        retVal->setMinimumDate(QDate::currentDate());
    }

    return retVal;
}
}
}
