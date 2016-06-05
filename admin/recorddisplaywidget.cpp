#include "recorddisplaywidget.h"

#include <QComboBox>
#include <QDebug>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVariantMap>

#include "data.h"

namespace paso {
namespace admin {

using namespace paso::data;

RecordDisplayWidget::RecordDisplayWidget(QWidget *parent) : QWidget(parent) {
    setLayout(new QFormLayout(this));
    layout()->setMargin(0);
    setMinimumWidth(320);
}

void RecordDisplayWidget::setupForRecord(
    const QSqlRecord &record, const QMap<QString, FieldType> &fieldEntryTypes) {
    mFieldEntryTypes = fieldEntryTypes;
    auto l = dynamic_cast<QFormLayout *>(layout());
    l->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    for (int i = 0; i < record.count(); i++) {
        QLabel *label =
            new QLabel(columnLabels[record.fieldName(i)].toString(), this);
        QWidget *edit = createWidgetForField(record, i);
        label->setBuddy(edit);
        mEditFields[record.fieldName(i)] = edit;
        l->addRow(label, edit);
    }
    mButtonBox = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    l->addWidget(mButtonBox);
    mButtonBox->setVisible(false);
    connect(mButtonBox, &QDialogButtonBox::accepted, this,
            &RecordDisplayWidget::accepted);
    connect(mButtonBox, &QDialogButtonBox::rejected, this,
            &RecordDisplayWidget::rejected);
}

QWidget *RecordDisplayWidget::createWidgetForField(const QSqlRecord &record,
                                                   int index) {
    auto fieldType = mFieldEntryTypes[record.fieldName(index)];
    QWidget *fieldEditor;
    switch (fieldType) {
    case FieldType::ComboBox: {
        auto combo = new QComboBox(this);
        for (const auto &role : enumeratedRoles.keys()) {
            combo->addItem(enumeratedRoles[role], roleToString(role));
        }
        combo->setEnabled(false);
        combo->setCurrentIndex(-1);
        fieldEditor = combo;
    } break;
    case FieldType::LineEdit: {
        auto edit = new QLineEdit(this);
        edit->setReadOnly(true);
        fieldEditor = edit;
    } break;
    case FieldType::PasswordEdit: {
        auto pwdEdit = new QLineEdit(this);
        pwdEdit->setReadOnly(true);
        pwdEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
        fieldEditor = pwdEdit;
    } break;
    default:
        fieldEditor = new QWidget(this);
    }
    fieldEditor->setSizePolicy(QSizePolicy::Policy::Expanding,
                               QSizePolicy::Policy::Fixed);
    return fieldEditor;
}

void RecordDisplayWidget::onDisplayRecord(const QSqlRecord &record) {
    for (int i = 0; i < record.count(); i++) {
        switch (mFieldEntryTypes[record.fieldName(i)]) {
        case FieldType::ComboBox: {
            auto field =
                dynamic_cast<QComboBox *>(mEditFields[record.fieldName(i)]);
            field->setCurrentIndex(field->findData(record.value(i)));
        } break;
        case FieldType::LineEdit:
        case FieldType::PasswordEdit:
            dynamic_cast<QLineEdit *>(mEditFields[record.fieldName(i)])
                ->setText(record.value(i).toString());
        }
    }
}

void RecordDisplayWidget::onEditRecord(QSqlRecord record) {
    mRecord = record;
    onDisplayRecord(record);
    setFieldsEditable();
}

void RecordDisplayWidget::clearData() {
    for (const auto &key : mFieldEntryTypes.keys()) {
        switch (mFieldEntryTypes[key]) {
        case FieldType::ComboBox:
            dynamic_cast<QComboBox *>(mEditFields[key])->setCurrentIndex(-1);
            break;
        case FieldType::LineEdit:
        case FieldType::PasswordEdit:
            dynamic_cast<QLineEdit *>(mEditFields[key])->clear();
            break;
        }
    }
}

void RecordDisplayWidget::saveSuccessfull() {
    setFieldsReadOnly();
    mRecord.clear();
}

void RecordDisplayWidget::saveError() {}

void RecordDisplayWidget::accepted() {
    for (const auto &key : mEditFields.keys()) {
        auto field = mEditFields[key];
        auto fieldType = mFieldEntryTypes[key];
        QString value;
        switch (fieldType) {
        case FieldType::ComboBox:
            value = dynamic_cast<QComboBox *>(field)->currentData().toString();
            break;
        case FieldType::LineEdit:
        case FieldType::PasswordEdit:
            value = dynamic_cast<QLineEdit *>(field)->text();
            break;
        }

        mRecord.setValue(key, value);
    }
    emit requestSave(mRecord);
}

void RecordDisplayWidget::rejected() {
    onDisplayRecord(mRecord);
    setFieldsReadOnly();
    emit editFinished();
}

void RecordDisplayWidget::setFieldsEditable() {
    for (const auto &key : mEditFields.keys()) {
        auto field = mEditFields[key];
        auto fieldType = mFieldEntryTypes[key];
        switch (fieldType) {
        case FieldType::ComboBox:
            field->setEnabled(true);
            break;
        case FieldType::LineEdit: {
            auto edit = dynamic_cast<QLineEdit *>(field);
            edit->setReadOnly(false);
        } break;
        case FieldType::PasswordEdit: {
            auto pwdEdit = dynamic_cast<QLineEdit *>(field);
            pwdEdit->setReadOnly(false);
            pwdEdit->setEchoMode(QLineEdit::Normal);
        } break;
        default:
            break;
        }
    }
    mButtonBox->setVisible(true);
}

void RecordDisplayWidget::setFieldsReadOnly() {
    for (const auto &key : mEditFields.keys()) {
        auto field = mEditFields[key];
        auto fieldType = mFieldEntryTypes[key];
        switch (fieldType) {
        case FieldType::ComboBox:
            field->setEnabled(false);
            break;
        case FieldType::LineEdit: {
            auto edit = dynamic_cast<QLineEdit *>(field);
            edit->setReadOnly(true);
        } break;
        case FieldType::PasswordEdit: {
            auto pwdEdit = dynamic_cast<QLineEdit *>(field);
            pwdEdit->setReadOnly(true);
            pwdEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
        } break;
        default:
            break;
        }
    }
    mButtonBox->setVisible(false);
}
}
}
