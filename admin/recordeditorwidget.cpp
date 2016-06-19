#include "recordeditorwidget.h"

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

RecordEditorWidget::RecordEditorWidget(const QSqlRecord &record,
                                       const FieldTypes &fieldTypes,
                                       QWidget *parent)
    : QWidget(parent), mFieldTypes(fieldTypes), mValidator(nullptr),
      mNewRecord(false), mFirstResponder(nullptr) {}

void RecordEditorWidget::setupUi(const QVariantMap &columnLabels,
                                 const QSqlRecord &record) {
    setLayout(new QFormLayout(this));
    layout()->setMargin(0);
    setMinimumWidth(320);
    auto l = dynamic_cast<QFormLayout *>(layout());
    l->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    for (int i = 0; i < record.count(); i++) {
        if (record.fieldName(i).toUpper() == "ID") {
            continue;
        }
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
            &RecordEditorWidget::accepted);
    connect(mButtonBox, &QDialogButtonBox::rejected, this,
            &RecordEditorWidget::rejected);
}

QWidget *RecordEditorWidget::createWidgetForField(const QSqlRecord &record,
                                                  int index) {
    auto fieldType = mFieldTypes[record.fieldName(index)];
    QWidget *fieldEditor;
    switch (fieldType) {
    case FieldType::ComboBox:
        fieldEditor = createComboBoxForRecordField(record.fieldName(index));
        break;
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

void RecordEditorWidget::onDisplayRecord(const QSqlRecord &record) {
    for (int i = 0; i < record.count(); i++) {
        if (record.fieldName(i).toUpper() == "ID") {
            continue;
        }
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

void RecordEditorWidget::onEditExistingRecord(QSqlRecord record) {
    mRecord = record;
    mNewRecord = false;
    prepareEdit(record);
    onDisplayRecord(record);
    setFieldsEditable();
}

void RecordEditorWidget::onEditNewRecord(QSqlRecord record) {
    mRecord = record;
    mNewRecord = true;
    prepareEdit(record);
    onDisplayRecord(record);
    setFieldsEditable();
}

void RecordEditorWidget::clearData() {
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

void RecordEditorWidget::saveSuccessfull() {
    setFieldsReadOnly();
    mRecord.clear();
}

void RecordEditorWidget::saveError() {}

void RecordEditorWidget::setValidator(RecordValidator *validator) {
    mValidator = validator;
}

const FieldEditors &RecordEditorWidget::fieldEditors() const {
    return mFieldEditors;
}

const FieldTypes &RecordEditorWidget::fieldTypes() const { return mFieldTypes; }

void RecordEditorWidget::accepted() {
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

void RecordEditorWidget::rejected() {
    if (mNewRecord) {
        clearData();
    } else {
        onDisplayRecord(mRecord);
    }
    setFieldsReadOnly();
    emit editFinished();
}

void RecordEditorWidget::setFieldsEditable() {
    for (const auto &key : mFieldEditors.keys()) {
        auto field = mFieldEditors[key];
        auto fieldType = mFieldTypes[key];
        auto readOnly = fieldReadOnly(key);
        switch (fieldType) {
        case FieldType::ComboBox:
            field->setEnabled(!readOnly);
            break;
        case FieldType::LineEdit: {
            auto edit = dynamic_cast<QLineEdit *>(field);
            // Handle special case where root username cannot be changed.
            edit->setReadOnly(readOnly);
        } break;
        case FieldType::PasswordEdit: {
            auto pwdEdit = dynamic_cast<QLineEdit *>(field);
            pwdEdit->setReadOnly(readOnly);
            pwdEdit->setEchoMode(QLineEdit::Normal);
        } break;
        default:
            break;
        }
    }
    mButtonBox->setVisible(true);
    this->setFocus();
    focusFirstEditable();
}

void RecordEditorWidget::setFieldsReadOnly() {
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

void RecordEditorWidget::focusFirstEditable() {
    for (auto child : children()) {
        auto lineEdit = dynamic_cast<QLineEdit *>(child);
        if (lineEdit != nullptr && !lineEdit->isReadOnly()) {
            lineEdit->setFocus();
            return;
        }
        auto combo = dynamic_cast<QComboBox *>(child);
        if (combo != nullptr && combo->isEnabled()) {
            combo->setFocus();
            return;
        }
    }

    mFirstResponder->setFocus();
}
}
}
