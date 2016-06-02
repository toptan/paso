#include "recorddisplaywidget.h"

#include <QVariantMap>
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QDebug>

#include "data.h"

namespace paso {
namespace admin {

using namespace paso::data;

RecordDisplayWidget::RecordDisplayWidget(QWidget *parent) : QWidget(parent) {
    setLayout(new QFormLayout(this));
    layout()->setMargin(0);
    setMinimumWidth(320);
}

void RecordDisplayWidget::setupForRecord(const QSqlRecord &record) {
    auto l = dynamic_cast<QFormLayout *>(layout());
    l->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    for (int i = 0; i < record.count(); i++) {
        QLabel *label =
            new QLabel(columnLabels[record.fieldName(i)].toString(), this);
        QWidget *edit = createWidgetForField(record, i);
        label->setBuddy(edit);
        edit->setSizePolicy(QSizePolicy::Policy::Expanding,
                            QSizePolicy::Policy::Fixed);
        mEditFields[record.fieldName(i)] = edit;
        l->addRow(label, edit);
    }
    mButtonBox = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    l->addWidget(mButtonBox);
    mButtonBox->setVisible(false);
    connect(mButtonBox, &QDialogButtonBox::accepted, this, &RecordDisplayWidget::accepted);
    connect(mButtonBox, &QDialogButtonBox::rejected, this, &RecordDisplayWidget::rejected);
}

QWidget *RecordDisplayWidget::createWidgetForField(const QSqlRecord &record,
                                                   int index) {
    QString field = record.fieldName(index);
    if (field == "role") {
        auto combo = new QComboBox(this);
        for (const auto &role : enumeratedRoles.keys()) {
            combo->addItem(enumeratedRoles[role], roleToString(role));
        }
        combo->setEnabled(false);
        combo->setCurrentIndex(-1);
        return combo;
    }

    QLineEdit *edit = new QLineEdit(this);
    if (field == "password") {
        edit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    }
    edit->setReadOnly(true);
    return edit;
}

void RecordDisplayWidget::onDisplayRecord(const QSqlRecord &record) {
    for (int i = 0; i < record.count(); i++) {
        {
            auto field =
                dynamic_cast<QLineEdit *>(mEditFields[record.fieldName(i)]);
            if (field != nullptr) {
                field->setText(record.value(i).toString());
            }
        }
        {
            auto field =
                dynamic_cast<QComboBox *>(mEditFields[record.fieldName(i)]);
            if (field != nullptr) {
                auto index = field->findData(record.value(i));
                if (index != -1) {
                    field->setCurrentIndex(index);
                }
            }
        }
    }
}

void RecordDisplayWidget::onEditRecord(QSqlRecord record) {
    onDisplayRecord(record);
    mRecord = record;
    for(auto key: mEditFields.keys()) {
        auto field = mEditFields[key];
        if (key == "role") {
            field->setEnabled(true);
        } else if (key == "password") {
            dynamic_cast<QLineEdit *>(field)->setEchoMode(QLineEdit::Normal);
            dynamic_cast<QLineEdit *>(field)->setReadOnly(false);
        } else if (dynamic_cast<QLineEdit *>(field) != nullptr) {
            dynamic_cast<QLineEdit *>(field)->setReadOnly(false);
        }
    }
    mButtonBox->setVisible(true);
}

void RecordDisplayWidget::clearData() {
    for (auto &field: mEditFields.values()) {
        if (dynamic_cast<QLineEdit *>(field) != nullptr) {
            dynamic_cast<QLineEdit *>(field)->clear();
        } else if (dynamic_cast<QComboBox *>(field) != nullptr) {
            dynamic_cast<QComboBox *>(field)->setCurrentIndex(-1);
        }
    }
}

void RecordDisplayWidget::accepted() {

}

void RecordDisplayWidget::rejected() {
    for(auto key: mEditFields.keys()) {
        auto field = mEditFields[key];
        if (key == "role") {
            field->setEnabled(false);
        } else if (key == "password") {
            dynamic_cast<QLineEdit *>(field)->setEchoMode(QLineEdit::PasswordEchoOnEdit);
            dynamic_cast<QLineEdit *>(field)->setReadOnly(true);
        } else if (dynamic_cast<QLineEdit *>(field) != nullptr) {
            dynamic_cast<QLineEdit *>(field)->setReadOnly(true);
        }
    }
    mButtonBox->setVisible(false);
    emit editCanceled();
}

}
}
