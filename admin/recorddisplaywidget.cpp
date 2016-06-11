#include "recorddisplaywidget.h"

#include <QComboBox>
#include <QDebug>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVariantMap>
#include <QToolTip>

#include "data.h"

namespace paso {
namespace admin {

using namespace paso::data;
using namespace std;

RecordDisplayWidget::RecordDisplayWidget(QWidget *parent)
    : QWidget(parent), mValidator(nullptr), mNewRecord(false),
      mEditingRootSystemUser(false), mFirstResponder(nullptr) {
    setLayout(new QFormLayout(this));
    layout()->setMargin(0);
    setMinimumWidth(320);
}

void RecordDisplayWidget::setupForRecord(
    const QSqlRecord &record, const QMap<QString, FieldType> &fieldEntryTypes) {
    mFieldTypes = fieldEntryTypes;
    auto l = dynamic_cast<QFormLayout *>(layout());
    l->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    for (int i = 0; i < record.count(); i++) {
        QLabel *label =
            new QLabel(columnLabels[record.fieldName(i)].toString(), this);
        QWidget *edit = createWidgetForField(record, i);
        label->setBuddy(edit);
        mFieldEditors[record.fieldName(i)] = edit;
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
    auto fieldType = mFieldTypes[record.fieldName(index)];
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
    if (index == 0) {
        mFirstResponder = fieldEditor;
    }
    return fieldEditor;
}

void RecordDisplayWidget::onDisplayRecord(const QSqlRecord &record) {
    for (int i = 0; i < record.count(); i++) {
        switch (mFieldTypes[record.fieldName(i)]) {
        case FieldType::ComboBox: {
            auto field =
                dynamic_cast<QComboBox *>(mFieldEditors[record.fieldName(i)]);
            field->setCurrentIndex(
                mNewRecord ? 0 : field->findData(record.value(i)));
        } break;
        case FieldType::LineEdit:
        case FieldType::PasswordEdit:
            dynamic_cast<QLineEdit *>(mFieldEditors[record.fieldName(i)])
                ->setText(record.value(i).toString());
        }
    }
}

void RecordDisplayWidget::onEditExistingRecord(QSqlRecord record) {
    mRecord = record;
    mNewRecord = false;
    mEditingRootSystemUser =
        mRecord.contains("username") && mRecord.value("username") == "root";
    onDisplayRecord(record);
    setFieldsEditable();
}

void RecordDisplayWidget::onEditNewRecord(QSqlRecord record) {
    mRecord = record;
    mNewRecord = true;
    mEditingRootSystemUser = false;
    onDisplayRecord(record);
    setFieldsEditable();
}

void RecordDisplayWidget::clearData() {
    for (const auto &key : mFieldTypes.keys()) {
        switch (mFieldTypes[key]) {
        case FieldType::ComboBox:
            dynamic_cast<QComboBox *>(mFieldEditors[key])->setCurrentIndex(-1);
            break;
        case FieldType::LineEdit:
        case FieldType::PasswordEdit:
            dynamic_cast<QLineEdit *>(mFieldEditors[key])->clear();
            break;
        }
    }
}

void RecordDisplayWidget::saveSuccessfull() {
    setFieldsReadOnly();
    mRecord.clear();
}

void RecordDisplayWidget::saveError() {}

void RecordDisplayWidget::setValidator(RecordValidator *validator) {
    mValidator = validator;
}

const FieldEditors &RecordDisplayWidget::fieldEditors() const {
    return mFieldEditors;
}

const FieldTypes &RecordDisplayWidget::fieldTypes() const {
    return mFieldTypes;
}

void RecordDisplayWidget::accepted() {
    shared_ptr<ValidationError> error;
    if (mValidator != nullptr) {
        error = mValidator->validate(mRecord);
    }

    if (error) {
        showEntryError(error->editor, error->title, error->text,
                       error->detailedText, error->icon);
        return;
    }

    for (const auto &key : mFieldEditors.keys()) {
        auto field = mFieldEditors[key];
        auto fieldType = mFieldTypes[key];
        QString value;
        switch (fieldType) {
        case FieldType::ComboBox:
            value = dynamic_cast<QComboBox *>(field)->currentData().toString();
            break;
        case FieldType::LineEdit:
        case FieldType::PasswordEdit:
            value = dynamic_cast<QLineEdit *>(field)->text().trimmed();
            break;
        }

        mRecord.setValue(key, value);
    }
    if (mNewRecord) {
        emit requestSave(mRecord);
    } else {
        emit requestUpdate(mRecord);
    }
}

void RecordDisplayWidget::rejected() {
    if (mNewRecord) {
        clearData();
    } else {
        onDisplayRecord(mRecord);
    }
    setFieldsReadOnly();
    emit editFinished();
}

void RecordDisplayWidget::setFieldsEditable() {
    for (const auto &key : mFieldEditors.keys()) {
        auto field = mFieldEditors[key];
        auto fieldType = mFieldTypes[key];
        switch (fieldType) {
        case FieldType::ComboBox:
            field->setEnabled(true);
            break;
        case FieldType::LineEdit: {
            auto edit = dynamic_cast<QLineEdit *>(field);
            // Handle special case where root username cannot be changed.
            edit->setReadOnly(key == "username" && mEditingRootSystemUser);
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
    this->setFocus();
    mFirstResponder->setFocus();
}

void RecordDisplayWidget::setFieldsReadOnly() {
    for (const auto &key : mFieldEditors.keys()) {
        auto field = mFieldEditors[key];
        auto fieldType = mFieldTypes[key];
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
